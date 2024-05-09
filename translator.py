from dataclasses import dataclass, field
from sys import argv
from io import StringIO
from re import compile

# namespace opcodes {
#    enum Opcodes {
#        mutate_attack = -19,        // mutate attack         = -19
#        mutate,                     // mutate genome         = -18
#        check_target_minerals,      // check target minerals = -17
#        check_my_minerals,          // check my minerals     = -16
#        check_my_level,             // check my level        = -15
#        check_my_energy,            // check my energy       = -14
#        align,                      // align horizontally    = -13
#        share,                      // share                 = -12
#        reproduction,               // reproduction          = -11
#        eat,                        // eat                   = -10
#        steal,                      // steal                 = -9
#        check,                      // check                 = -8
#        minerals,                   // minerals              = -7
#        photosynthesis,             // photosynthesis        = -6
#        convert_minerals,           // convert minerals      = -5
#        change_dir,                 // change directory (absolutely) = -4
#        right,                      // right                 = -3
#        left,                       // left                  = -2
#        step                        // step                  = -1
#   };
#}

#class Opcode(Enum):
#    MUTATE_ATTACK         = -19
#    MUTATE                = -18
#    CHECK_TARGET_MINERALS = -17
#    CHECK_MY_MINERALS     = -16
#    CHECK_MY_LEVEL        = -15
#    CHECK_MY_ENERGY       = -14
#    ALIGN                 = -13
#    SHARE                 = -12
#    REPRODUCTION          = -11
#    EAT                   = -10
#    STEAL                 = -9
#    CHECK                 = -8
#    MINERALS              = -7
#    PHOTOSYNTHESIS        = -6
#    CONVERT_MINERALS      = -5
#    CHANGE_DIR            = -4
#    RIGHT                 = -3
#    LEFT                  = -2
#    STEP                  = -1

@dataclass
class Opcode:
    number: int = 0
    arguments_count: int = 0
    results_count: int = 0

@dataclass
class GoTo(Opcode):
    pointer: str = ''

Opcodes = {
    'MUTATE_ATTACK': Opcode(-19, 0, 0),
    'MUTATE': Opcode(-18, 0, 0),
    'CHECK_TARGET_MINERALS': Opcode(-17, 0, 1),
    'CHECK_MY_MINERALS': Opcode(-16, 1, 1),
    'CHECK_MY_LEVEL': Opcode(-15, 1, 1),
    'CHECK_MY_ENERGY': Opcode(-14, 1, 1),
    'ALIGN': Opcode(-13, 0, 0),
    'SHARE': Opcode(-12, 1, 0),
    'REPRODUCTION': Opcode(-11, 0, 0),
    'EAT': Opcode(-10, 0, 0),
    'STEAL': Opcode(-9, 0, 0),
    'CHECK': Opcode(-8, 0, 4),
    'MINERALS': Opcode(-7, 0, 0),
    'PHOTOSYNTHESIS': Opcode(-6, 0, 0),
    'CONVERT_MINERALS': Opcode(-5, 0, 0),
    'CHANGE_DIR': Opcode(-4, 1, 0),
    'RIGHT': Opcode(-3, 0, 0),
    'LEFT': Opcode(-2, 0, 0),
    'STEP': Opcode(-1, 0, 0),
    'NOP': Opcode(0, 0, 0)
}

# обработка директив
# лексический анализ
# начальная обработка команд
# подстановка ссылок
# сохранение

def translate_gen_to_byte(gen):
    if gen < 0:
        return ~gen ^ 0xFF
    return gen

@dataclass
class Command:
    opcode: Opcode
    arguments: list[int] = field(default_factory=list)
    handlers: list[int] = field(default_factory=list)
    point: int = None

    def __len__(self):
        return 1 + len(self.arguments) + sum(map(len, self.handlers))

    def __bytes__(self):
        return bytes(map(translate_gen_to_byte, [self.opcode.number] + self.arguments + list(b''.join(map(bytes, self.handlers)))))


GENOME_SIZE = 60
DEFAULT_COMMAND = 'NOP'
COMMAND_PATTERN = compile(r'(\w+)\s*(?:\((.*)\))?\s*(?:{(.*)})?')


class PreProcessor:
    defines = {}

    def handle_directive_line(self, line):
        parsed = line.removeprefix('#').split()
        match parsed:
            case 'define', a, b:
                self.defines[a] = b
            case 'size', num:
                GENOME_SIZE = int(num)
            case 'default', op:
                DEFAULT_COMMAND = op

    @staticmethod
    def remove_comment(line):
        if '//' in line:
            return line[:line.find('//')]
        return line

    def handle_file(self, filename):
        result = StringIO()
        file = open(filename, 'r')
        for line in file:
            line = line.strip().upper()
            for key, value in self.defines.values():
                line = line.replace(key, value)
            if line.startswith('#'):
                self.handle_directive_line(line)
                continue
            line = PreProcessor.remove_comment(line)
            if line:
                result.write(line)
        file.close()
        result.seek(0)
        return result


class LexicalAnalysis:
    def __init__(self, buffer):
        self.buffer = buffer

    def tokenize(self):
        tokens = []
        command = ''
        while True:
            sym = self.buffer.read(1)
            if not sym:
                return tokens
            if sym in (';', ':'):
                tokens.append(command if sym != ':' else command + ':')
                command = ''
            else:
                command += sym


class CommandProcessor:
    point = 0
    pointers = {}
    commands = []

    def __init__(self, tokens):
        self.tokens = tokens

    def save_point(self, name, point):
        self.pointers[name] = point

    @staticmethod
    def argumetns2ints(arguments):
        return list(map(int, arguments.split(',')))

    @staticmethod
    def process_command_handlers(handlers, point):
        subcommands = []
        handlers = handlers.split(',')
        result = []
        for i, handler in enumerate(handlers):
            subcommand = CommandProcessor.process_command(handler, point+i)
            if len(subcommand) > 1 and i != len(handlers)-1:
                print('Error process handler, subcommand size > 1:', subcommand)
                exit(1)
            result.append(subcommand)
        return result

    @staticmethod
    def process_command(token, point):
        #print(token, point, end=' ')
        matched = COMMAND_PATTERN.fullmatch(token)
        if not matched:
            print('Error matching, token:', token)
            exit(1)
        name, arguments, command_handlers = matched.groups()
        #print(name, arguments, command_handlers, end=' ')
        if name == 'GOTO':
            return Command(GoTo(None, pointer=arguments), point=point)
        opcode = Opcodes[name]
        arguments = CommandProcessor.argumetns2ints(arguments) if arguments else [] # '1, 2, 3' -> [1, 2, 3]
        handlers = CommandProcessor.process_command_handlers(command_handlers, point+1+len(arguments)) if command_handlers else [] # 'GOTO A, GOTO B' -> [Command(...), Command(...)]
        command = Command(opcode, arguments, handlers, point=point)
        #print(command, len(command))
        return command

    def process(self):
        for token in self.tokens:
            if token.endswith(':'):
                self.save_point(token.removesuffix(':'), self.point)
                continue
            command = self.process_command(token, self.point)
            self.point += len(command)
            self.commands.append(command)

    def fill(self):
        command = CommandProcessor.process_command(DEFAULT_COMMAND, 0)
        if len(command) > 1:
            print('Error, default command size > 1:', command)
            exit(1)
        for i in range(self.point, GENOME_SIZE):
            self.commands.append(CommandProcessor.process_command(DEFAULT_COMMAND, i))
        self.point = GENOME_SIZE

    def calc_pointer_offset(self, point, dest):
        if dest not in self.pointers:
            print('Error, pointer not found:', dest)
            exit(1)
        dest = self.pointers[dest]
        return (dest + GENOME_SIZE - point) % GENOME_SIZE

    def process_command_pointers(self, command):
        if isinstance(command.opcode, GoTo):
            command.opcode.number = self.calc_pointer_offset(command.point, command.opcode.pointer) - 1
        else:
            for handler in command.handlers:
                self.process_command_pointers(handler)

    def process_pointers(self):
        for command in self.commands:
            self.process_command_pointers(command)



def save_genome(commands, filename):
    with open(filename, 'wb') as file:
        for command in commands:
            file.write(bytes(command))


def translate(in_filename, out_filename):
    buffer = PreProcessor().handle_file(in_filename)
    tokens = LexicalAnalysis(buffer).tokenize()
    print('Tokenizator:', tokens)
    commandprocessor = CommandProcessor(tokens)
    commandprocessor.process()
    commandprocessor.process_pointers()
    print('Command processor done!')
    print(commandprocessor.commands)
    commandprocessor.fill()
    save_genome(commandprocessor.commands, out_filename)


if __name__ == '__main__':
    in_filename = argv[1]
    out_filename = argv[2]
    translate(in_filename, out_filename)
