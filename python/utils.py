import ROOT as r

def LoadLibrary(path='lib',name='RootUtils'):
	if path[len(path)-1] == '/':
		libname = path+'lib'+name+'.so'
	else:
		libname = path + '/lib'+name+'.so'
	print 'Loading the library ' + name + ' from -->',libname
	r.gSystem.Load(libname)
