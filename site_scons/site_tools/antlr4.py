import re
import subprocess


def _ConstructAntlrCommandLine(src, env):
    dst_path = src.get_dir()
    src_path = src.srcnode().get_dir()
    return '$ANTLR4 -lib %s -o %s -Xexact-output-dir %s' % (src_path, dst_path, src)


def Antlr4FileEmitter(target, source, env):
    target = [] # Clear existing targets, since SCons wants to create one using the normal suffix rules
    for src in source:
        command = env.subst(_ConstructAntlrCommandLine(src, env) + ' -depend')
        output = subprocess.check_output(command.split(), universal_newlines=True).split('\n')
        for line in output:
            i = line.find(' : ')
            if i != -1:
                dst = line[0:i]
                target.append(dst)

    return (target, source)


def Antlr4FileScanner(node, env, path):
    importRE = re.compile(r'^\s*import\s+(\S+)\s*;', re.MULTILINE)

    libdir = node.srcnode().get_dir()
    contents = node.get_text_contents()
    imports = re.findall(importRE, contents)
    grammars = []
    for i in imports:
        grammars.append(libdir.File(i + '.g4'))
    return grammars


def Antlr4FileActionGenerator(source, target, env, for_signature):
    commands = []
    for src in source:
        commands.append(_ConstructAntlrCommandLine(src, env))
    return commands


def generate(env):
    # Scanner for determining grammar file dependencies
    antlrScanner = env.Scanner(
        function = Antlr4FileScanner,
        skeys = ['.g4']
    )
    env.Append(SCANNERS = antlrScanner)

    # Builder + emitter for antlr files
    from SCons.Builder import Builder
    builder = Builder(generator = Antlr4FileActionGenerator,
                      src_suffix = '.g4',
                      emitter = Antlr4FileEmitter)
    env.Append(BUILDERS = {'Antlr4Grammar':builder})

    # Path to the antlr executable
    env.Replace(ANTLR4 = env.File('#/external/jdk-12/bin/java.exe').get_abspath() + ' -jar ' + env.File('#/external/antlr-4.7.1-complete.jar').get_abspath())


def exists(env):
   """
   Make sure antlr exists.
   """
   return env.Detect("antlr")
