#
# Copyright 2010-2013 Fabric Engine Inc. All rights reserved.
#

import os, sys, platform, copy, glob

Import(
  'parentEnv',
  'FABRIC_DIR',
  'FABRIC_SPLICE_VERSION',
  'STAGE_DIR',
  'FABRIC_BUILD_OS',
  'FABRIC_BUILD_TYPE',
  'QT_INCLUDE_DIR',
  'QT_LIB_DIR',
  'sharedCapiFlags',
  'spliceFlags'
  )

qtDir = os.path.split(QT_INCLUDE_DIR)[0]
if FABRIC_BUILD_OS == 'Linux':
  qtDir = '/usr'
if FABRIC_BUILD_OS == 'Darwin':
  qtDir = '/usr/local'

# create the build environment
env = Environment(MSVC_VERSION='10.0', tools=['default','qt'], QTDIR=qtDir, QT_LIB='', ENV=parentEnv['ENV'])
if FABRIC_BUILD_OS == 'Linux':
  env.Replace(QT_MOC = '$QT_BINPATH/moc-qt4')

env.Append(CC = parentEnv['CC'])
env.Append(CXX = parentEnv['CXX'])
env.Append(CCFLAGS = parentEnv['CCFLAGS'])
env.Append(LINKFLAGS = parentEnv['LINKFLAGS'])
env.Append(CPPDEFINES = parentEnv['CPPDEFINES'])
env.Append(CPPPATH = parentEnv['CPPPATH'])
env.Append(LIBPATH = parentEnv['LIBPATH'])
env.Append(LIBS = parentEnv['LIBS'])

qtFlags = {
  'CPPPATH': [
    QT_INCLUDE_DIR,
    os.path.join(QT_INCLUDE_DIR, 'Qt')
    ],
  }
if FABRIC_BUILD_OS == 'Darwin':
  qtFlags['FRAMEWORKPATH'] = [QT_LIB_DIR]
else:
  qtFlags['LIBPATH'] = [QT_LIB_DIR]

libSuffix = ''
if FABRIC_BUILD_OS == 'Windows':
  libSuffix = '4'
  if FABRIC_BUILD_TYPE == 'Debug':
    libSuffix = 'd' + libSuffix
if FABRIC_BUILD_OS == 'Windows':
  qtFlags['CCFLAGS'] = ['/DNT_PLUGIN']
elif FABRIC_BUILD_OS == 'Linux':
  qtFlags['CCFLAGS'] = ['-DLINUX']

libs = ['QtCore'+libSuffix, 'QtGui'+libSuffix, 'QtOpenGL'+libSuffix]
if FABRIC_BUILD_OS == 'Darwin':
  qtFlags['FRAMEWORKS'] = libs
else:
  qtFlags['LIBS'] = libs

if FABRIC_BUILD_OS == 'Windows':
  env.Append(LIBS = ['advapi32', 'shell32', 'user32', 'Opengl32', 'glu32', 'gdi32'])
if FABRIC_BUILD_OS == 'Linux' or FABRIC_BUILD_OS == 'Darwin':
  env.Append(LIBS = ['boost_program_options'])
if FABRIC_BUILD_OS == 'Linux':
  env.Append(LIBS = ['pthread'])

env.MergeFlags(qtFlags)
env.MergeFlags(sharedCapiFlags)
env.MergeFlags(spliceFlags)

env.Append(CPPPATH = [env.Dir('src')])

def GlobRecursive(self, pattern, useBuildDir = False):
  (folder, file) = os.path.split(pattern)
  if not folder:
    folder = '.'

  if not os.path.isabs(folder):
    folder = self.Dir(folder).srcnode().abspath

  if useBuildDir:
    if os.path.isabs(folder):
      pattern = os.path.join(os.path.relpath(folder, env.Dir('.').srcnode().abspath), file)

  result = Glob(pattern)
  for root, dirnames, filenames in os.walk(folder):
    doneDir = {'.': True}
    for dirname in dirnames:
      if doneDir.has_key(dirname):
        continue
      doneDir[dirname] = True
      result += self.GlobRecursive(os.path.join(root, dirname, file), useBuildDir = useBuildDir)


  result = self.Flatten(result)
  return result
env.AddMethod(GlobRecursive)

sources = env.Glob('src/*.cpp')
sources += env.Glob('src/Widgets/*.cpp')
sources += env.Glob('src/Widgets/AE/*.cpp')

target = 'splice'

if FABRIC_BUILD_OS == 'Linux':
  env.Append(LINKFLAGS = [Literal('-Wl,-rpath,$ORIGIN/../lib')])
  env['_LIBFLAGS' ] = '-Wl,--start-group ' + env['_LIBFLAGS'] + ' -Wl,--end-group'
if FABRIC_BUILD_OS == 'Windows':
  env.Append(LINKFLAGS = ['/STACK:67108864'])
if FABRIC_BUILD_OS == 'Darwin':
  env.Append(LINKFLAGS = ['-Wl,-rpath,@loader_path/..'])

samplesDir = STAGE_DIR.Dir('Samples').Dir('SpliceStandalone')
binDir = STAGE_DIR.Dir('bin')

standaloneFiles = []
if FABRIC_BUILD_OS == 'Windows':
  standaloneApp = env.Program(binDir.File('splice.exe'), sources)
else:
  standaloneApp = env.Program(binDir.File('splice'), sources)
standaloneFiles.append(standaloneApp)

for sampleFile in glob.glob(os.path.join(env.Dir('samples').srcnode().abspath, '*')):
  baseName = os.path.split(sampleFile)[1]
  if os.path.isfile(sampleFile):
    standaloneFiles.append(env.Install(samplesDir, env.File(baseName)))
  else:
    standaloneFiles.append(env.Install(samplesDir.Dir(baseName), env.Glob('samples/%s/*' % baseName)))

if FABRIC_BUILD_OS == 'Windows':
  qtBinDir = os.path.join(os.path.split(QT_LIB_DIR)[0], 'bin')
  for qtLib in qtFlags['LIBS']:
    standaloneFiles.append(env.Install(binDir, env.Glob(os.path.join(qtBinDir, '*%s*.dll' % qtLib))))

standaloneFiles.append(
  env.Install(
    STAGE_DIR.Dir('Resources'),
    Glob(os.path.join('images', '*.jpg')) + 
    Glob(os.path.join('images', '*.png'))
    )
  )

# [pzion 20141001] WTF???
# # install PDB files on windows
# if FABRIC_BUILD_TYPE == 'Debug' and FABRIC_BUILD_OS == 'Windows':
#   env['CCPDBFLAGS']  = ['${(PDB and "/Fd%s_incremental.pdb /Zi" % File(PDB)) or ""}']
#   pdbSource = standaloneApp[0].get_abspath().rpartition('.')[0]+".pdb"
#   pdbTarget = os.path.join(STAGE_DIR.abspath, os.path.split(pdbSource)[1])
#   copyPdb = env.Command( 'copy', None, 'copy "%s" "%s" /Y' % (pdbSource, pdbTarget) )
#   env.Depends( copyPdb, installedApp )
#   env.AlwaysBuild(copyPdb)

alias = env.Alias('splicestandalone', standaloneFiles)
spliceData = (alias, standaloneFiles)
Return('spliceData')
