#!/usr/bin/python
import argparse
import re

#Parse options
parser = argparse.ArgumentParser(description="Create SDCC compatible header from STC header file")

parser.add_argument('-f', action='store', dest="filename", help="Load from file")

options = parser.parse_args()

with open(options.filename) as f:
    lines = f.readlines()
    print """#ifndef STC15F204EA_H_20160714
#define STC15F204EA_H_20160714

"""

    print """
#if defined SDCC
#define DEFINE_SBIT(name, addr, bit)  __sbit  __at(addr+bit)   name
#define DEFINE_SFR(name, addr)        __sfr   __at(addr)       name
#define DEFINE_SFRX(name, addr)       __xdata volatile unsigned char __at(addr)    name
#define DEFINE_SFR16(name, addr)      __sfr16 __at(((addr+1U)<<8) | addr)     name
#define DEFINE_SFR16E(name, fulladdr) __sfr16 __at(fulladdr)   name
#define DEFINE_SFR32(name, addr)      __sfr32 __at(((addr+3UL)<<24) | ((addr+2UL)<<16) | ((addr+1UL)<<8) | addr) name
#define DEFINE_SFR32E(name, fulladdr) __sfr32 __at(fulladdr)   name

//#define SFR __sfr
#define SBIT __sbit
#define BIT __bit
#define INTERRUPT __interrupt
#define IDATA __idata
#define CODE __code

#endif
"""

    print """
/** Keil C51
  * http://www.keil.com
 */
#if defined __CX51__
#define DEFINE_SBIT(name, addr, bit)  sbit  name = addr^bit
#define DEFINE_SFR(name, addr)        sfr   name = addr
#define DEFINE_SFRX(name, addr)       volatile unsigned char xdata name _at_ addr
#define DEFINE_SFR16(name, addr)      sfr16 name = addr
#define DEFINE_SFR16E(name, fulladdr) /* not supported */
#define DEFINE_SFR32(name, fulladdr)  /* not supported */
#define DEFINE_SFR32E(name, fulladdr) /* not supported */

//#define SFR sfr
#define SBIT sbit
#define BIT sbit
#define INTERRUPT interrupt
#define IDATA idata
#define CODE code

# endif
"""

    for line in lines:
        if (re.match(r'^/w*$', line)):
            print line
            continue

        if (re.match(r'^//', line)):
            print line
            continue
        
        m = re.match(r'\s*sfr\s+(?P<name>\w+)\s+=\s*(?P<value>\S*)\s*;\s*(?P<comment>//.*)*', line)
        if m:
            if m.group('name') and m.group('value'):
                print "//%s" % line.rstrip()
                print "#define %s_INITVAL (%s)" % (m.group('name'), m.group('value'))
                print "DEFINE_SFR(%s, %s_INITVAL);\n" % (m.group('name'), m.group('name'))
                continue

        m = re.match(r'\s*sbit\s+(?P<name>\w+)\s+=\s*(?P<value>\S*)\^(?P<bit>\d)\s*;\s*(?P<comment>//.*)*', line)
        if m:
            if m.group('name') and m.group('value') and m.group('bit'):
                print "//%s" % line.rstrip()
                #print "#define %s (%s) %s" % (m.group('name'), m.group('value'), m.group('comment') if m.group('comment') else "")
                print "DEFINE_SBIT(%s, %s_INITVAL, %s);\n" % (m.group('name'), m.group('value'), m.group('bit'))
                continue

        print "//FIXME -- %s" % line.rstrip()

    print """
#endif
"""

