def testRemovedFunctions():
    def tryRemovedFunction(name, code):
        try:
            eval(code)
            print name, 'NOT removed'
        except NameError:
            print name, 'removed'

    tryRemovedFunction('execfile', 'execfile("/usr/lib/dangerous")')
    tryRemovedFunction('file', 'file("/etc/shadow")')
    tryRemovedFunction('open', 'open("/etc/shadow")')
    tryRemovedFunction('reload', 'reload(__builtin__)')

def testUnicode():
    print 'Did you say あ?'
    print unicode('あああああ!!')

def testImport():
    import math
    print 'π =', math.pi

    try:
        import os
        print 'module whitelist DOES NOT work'
    except SystemError:
        print 'module whitelist works'

