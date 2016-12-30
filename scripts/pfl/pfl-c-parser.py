#!/usr/bin/env python

import argparse
import re
import sys

def is_function_call(s):
  s=s.replace('{', '')
  b1=re.findall(r'(?<=(?<=int\s)|(?<=void\s)|(?<=string\s)|(?<=double\s)|(?<=float\s)|(?<=char\s)).*?(?=\s?\()', s)
  b2=re.findall(r"^\s*[\w_][\w\d_]*\s*.*\s*[\w_][\w\d_]*\s*\(.*\)\s*$", s)
  return b1 and b2


def contains(S, s):
  pos=S.find(s)
  if pos<0: return False
  pos2=S.find('//')
  if pos2>=0 and pos2<pos: return False
  pos3=S.find('/*')
  if pos3>=0 and pos3<pos: return False
  return True

def get_assert_cond(s):
  cond=s
  pos=cond.find('//')
  if pos>0:
    cond=cond[:pos]
  cond=cond.replace(' ', '')
  cond=cond.replace('assert(', '')
  cond=cond.replace(');', '')
  cond=cond.replace('\n', '')
  return cond

def count_assert(fname):
    count=0
    with open(fname) as f:
      for line in f:
        if contains(line, 'assert('): count=count+1
    return count


def main():
  parser=argparse.ArgumentParser(description='Convert C source code for PFL')
  parser.add_argument('file', metavar='file', action='store',
                      help='The source file to use')

  args=parser.parse_args()

  fname=args.file 

  res=[]
  
  ## to print the global declaration of '__CPROVER_fault'
  #res.append('int __CPROVER_fault0=1;\n')
  cprover_faults=[]
  with open(fname) as f:
    lnum=0
    for line in f:
      lnum=lnum+1
      if contains(line, 'assert('):
        res.append('int __CPROVER_fault'+str(lnum)+'=1;')
        cprover_faults.append('__CPROVER_fault'+str(lnum))
  res.append('\n')

  with open(fname) as f:
    lnum=0
    is_main=False
    main_return=''
    for line in f:
      if is_function_call(line) and line.find('main')>=0:
        #print line + " is function call"
        is_main=True
      lnum=lnum+1
      if contains(line, 'assert('):
        cond=get_assert_cond(line)
        new_str='if(!(' + cond + ')) __CPROVER_fault' + str(lnum) + '=0;'
        res.append(new_str)
        # add the __CPROVER_faults
        line_info='line ' + str(lnum)
        x= '__CPROVER_fault' + str(lnum)
        #res.append('  __CPROVER_assert('+x+'==1, \"__CPROVER_fault failing traces: ' + line_info + '\");\n')
        #res.append('  __CPROVER_assert('+x+'==0, \"__CPROVER_fault passing traces: ' + line_info + '\");\n')
        flag='flag' + str(lnum)
        res.append('_Bool ' + flag + ';')
        res.append('  if(' + flag + ') __CPROVER_assert('+x+'==0, \"__CPROVER_fault passing traces: ' + line_info + '\");')
        res.append('  else  __CPROVER_assert('+x+'==1, \"__CPROVER_fault failing traces: ' + line_info + '\");\n')
      elif is_main and line.find('{')>=0:
        line=line.replace('{', '{if(1) {int __CPROVER_dummy=1;}')
        res.append(line)
        #res.append('if(1) {int __CPROVER_dummy=1;}')
        #res.append('if(1) {__CPROVER_fault0=1;}\n')
        is_main=False
      else:
        res.append(line)

    for l in res:
      sys.stdout.write(l) #print l


if __name__=="__main__":
  main()
