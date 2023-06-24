from sys import exit

instructions = [["nop", "ei", "di", "int", ["in", "$", "*"], ["out", "*", "$"], "jmp", "cll", "ret", "hlt", "rst"], 
[["str", "*$", "%"], ["ldr", "$%", "*"], "mov", ["push", "*$"], "pop", "swp"],
["add", "sub", "mul", "div", "and", "or", "xor", "neg", "rsl", "rsr", "cmp", "inc", "dcr"]]
registers = ["r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "ip", "bp", "sp", "flag"]
conditions = ["uc", "eq", "ne", "cs", "cc", "sn", "sp", "os", "oc", "uh", "ul", "sh", "sl", "sg", "su"]
miscTokens = []

widths = "bw"
regSelects = "hl"

debugImme = "Value passed was not an Immediate, maybe missed a '$'?"
debugReg = "Value passed was not a Register, please check again!"
debugAddr = "Value passed was not an Address, maybe missed a '%'?"

def debugPrint(a, b, msg):
    print(f"Line {a}, Arg {b}: " + msg + "\n")
    exit(-1)

def packInstruction(OpG, OpCode, Cond, I, C, W):
    return ((W << 15) | (C << 13) | (I << 12) | (Cond << 7) | (OpCode << 3) | OpG)

def tokenise(line): # Translate input into relevant tokens
    if not line:
        return
    tokens = []

    if line[0][-1] == ':':
        if line[0][:-1] in miscTokens:
            tokens.append('^' + str(miscTokens.index(line[0][:-1])))
        else:
            miscTokens.append(line[0][:-1])
            tokens.append('^' + str(len(miscTokens) - 1))
        line.pop(0)

    if not line:
        return tokens
    
    ins = line[0]
    op = [0, 0]
    cond = 0
    for a, condition in enumerate(conditions):
        if ins.find(condition) != -1:
            cond = a
            ins = ins.replace(condition, '')
            break
    for a, group in enumerate(instructions):
        if ins in group:
            op = [a, group.index(ins)]
            break
    tokens.append('&' + str(cond).zfill(2) + str(op[1]).zfill(2) + str(op[0]).zfill(1))
    line.pop(0)
    
    if line[0][0] == '!':
        f = False
        c = False
        w = 0
        metadata = line[0][1:].split()
        for flag in metadata:
            if flag == 'f' and not f:
                f = True
                continue
            if flag == 'c' and not c:
                c = True
                continue
            if flag in [*widths] and not w:
                w = [*widths].index(flag) + 1
                continue
        tokens.append('!' + str(int(f)).zfill(1) + str(int(c)).zfill(1) + str(w).zfill(1))
        line.pop(0)

    for a, word in enumerate(line):
        if word[0] == '$':
            tokens.append('$' + str(int(word[1:], 0)))
            continue
        elif word[0] == '%':
            tokens.append('%' + str(int(word[1:], 0)))
            continue
        elif word[0] in [*regSelects] and word[1:] in registers:
            tokens.append('*' + str([*regSelects].index(word[0])) + str(registers.index(word[1:])).zfill(2))
            continue
        elif word in registers:
            tokens.append('*' + str(0).zfill(0) + str(registers.index(word)).zfill(2))
            continue
        else:
            if word in miscTokens:
                tokens.append('^' + str(miscTokens.index(word)))
                continue
            else:
                miscTokens.append(word)
                tokens.append('^' + str(miscTokens.index(word)))

    return tokens

def translateLabels(a, line): # Translate all Label Tokens into correct address pointers
    tokens = []

    if not line:
        return
    if line[0][0] == '^':
        miscTokens[int(line[0][1:])] = '%' + str(a)
        line.pop(0)
    if len(line) >= 1:
        for word in line:
            if word[0] == '^' and miscTokens[int(word[1:])][0] == '%':
                tokens.append(miscTokens[int(word[1:])])
            else:
                tokens.append(word)

    return tokens

def checkInstructions(a, oline): # Check for unresolved labels left over from previous stage and check all instructions have required or valid inputs.
    line = oline.copy()
    if not line:
        return
    for b, word in enumerate(line):
        if word[0] == '^':
            debugPrint(a + 1, b, "Unresolved Label!")
    ins = [int(line[0][-1:]), int(line[0][-3:-1])]
    if line[1][0] == '!':
        line.pop(1)
    match ins[0]:
        case 0: # SYS
            match ins[1]:
                case 3: # INT
                    if line[1][0] != '$':
                        debugPrint(a + 1, 1, debugImme)
                    if int(line[1][1:]) > 255:
                        debugPrint(a + 1, 1, "Interrupt Value exceeds 8 bits, please correct this!")
                    return
                case 4: # IN
                    if not line[1][0] in [*instructions[0][4][1]]:
                        debugPrint(a + 1, 1, debugImme)
                    if int(line[1][1:]) > 255:
                        debugPrint(a + 1, 1, "Port Value exceeds 8 bits, please correct this!")
                    if not line[1][0] in [*instructions[0][4][2]]:
                        debugPrint(a + 1, 2, debugReg)
                    return
                case 5: # OUT
                    if not line[1][0] in [*instructions[0][5][1]]:
                        debugPrint(a + 1, 1, debugReg)
                    if not line[2][0] in [*instructions[0][5][2]]:
                        debugPrint(a + 1, 2, debugImme)
                    if int(line[2][1:]) > 255:
                        debugPrint(a + 1, 1, "Port Value exceeds 8 bits, please correct this!")
                    return
                case 6 | 7: # JMP / CLL
                    if not line[1][0] in [*"$%"]:
                        debugPrint(a + 1, 1, "Value passed was not an Address or an Immediate, maybe you missed a '$' or a '%'?")
                    return
                case _:
                    pass
        case 1: # DAT
            match ins[1]:
                case 0: # STR
                    if not line[1][0] in [*instructions[1][0][1]]:
                        debugPrint(a + 1, 1, "Value passed was not a Register or an Immediate, maybe you missed a '$'? If it was a Register, please check again!")
                    if not line[2][0] in [*instructions[1][0][2]]:
                        debugPrint(a + 1, 2, debugAddr)
                    return
                case 1: # LDR
                    if not line[1][0] in [*instructions[1][1][1]]:
                        debugPrint(a + 1, 1, "Value passed was not an Address or an Immediate, maybe you missed a '$' or a '%'?")
                    if not line[2][0] in [*instructions[1][1][2]]:
                        debugPrint(a + 1, 2, debugReg)
                    return
                case 2 | 5: # MOV / SWP
                    word = False
                    if line[1][0] != '*':
                        debugPrint(a + 1, 1, debugReg)
                    if line[1][1] == '0':
                        word = True
                    if line[2][0] != '*':
                        debugPrint(a + 1, 2, debugReg)
                    return
                case 3: # PUSH
                    if not line[1][0] in [*instructions[1][3][1]]:
                        debugPrint(a + 1, 1, "Value passed was not a Register or an Immediate, maybe you missed a '$'? If it was a Register, please check again!")
                    return
                case 4: # POP
                    if line[1][0] != '*':
                        debugPrint(a + 1, 1, debugReg)
                    return
        case 2: # ARH
            match ins[1]:
                case 7 | 8 | 9: # NEG / RSL / RSR
                    if not line[1][0] in [*"*$"]:
                        debugPrint(a + 1, 1, "Value passed was not a Register or an Immediate, maybe you missed a '$'? If it was a Register, please check again!")
                        if line[1][0] == '$' and len(line) < 2:
                            debugPrint(a + 1, 1, "Immediate used in Main without Destination Register!")
                    if len(line) > 2:
                        if line[2][0] != '*':
                            debugPrint(a + 1, 2, debugReg)
                    return
                case 8 | 9: # RSL / RSR
                    if line[1][0] != '*':
                        debugPrint(a + 1, 1, debugReg)
                    if len(line) > 2:
                        if line[2][0] != '*':
                            debugPrint(a + 1, 2, debugReg)
                    return
                case 10: # CMP
                    if not line[1][0] in [*"*$"]:
                        debugPrint(a + 1, 1, "Value passed was not a Register or an Immediate, maybe you missed a '$'? If it was a Register, please check again!")
                    if not line[2][0] in [*"*$"]:
                        debugPrint(a + 1, 2, "Value passed was not a Register or an Immediate, maybe you missed a '$'? If it was a Register, please check again!")
                    return
                case 11 | 12: # INC / DCR
                    if line[1][0] != '*':
                        debugPrint(a + 1, 1, debugReg)
                case _: # ADD / SUB / MUL / DIV / AND / OR / XOR
                    if not line[1][0] in [*"*$"]:
                        debugPrint(a + 1, 1, "Value passed was not a Register or an Immediate, maybe you missed a '$'? If it was a Register, please check again!")
                        if line[1][0] == '$' and len(line) < 2:
                            debugPrint(a + 1, 1, "Immediate used in Main without Destination Register!")
                    if not line[2][0] in [*"*$"]:
                        debugPrint(a + 1, 2, "Value passed was not a Register or an Immediate, maybe you missed a '$'? If it was a Register, please check again!")
                    if len(line) > 3:
                        if line[3][0] != '*':
                            debugPrint(a + 1, 3, debugReg)
                    return

    return

def assemble(line): # Satisfied that the contents of the program are valid, assemble the binary.
    data = []
    ins = [int(line[0][-1:]), int(line[0][-3:-1]), int(line[0][1:3])] # OpGroup, OpCode, Cond
    line.pop(0)
    meta = [0, 0, 0] # F C W
    if len(line) > 1:
        if line[0][0] == '!':
            meta = [int(line[0][1]), int(line[0][2]), int(line[0][3]) - 1]
            line.pop(0)
    
    match ins[0]:
        case 0: # SYS
            match ins[1]:
                case 3: # INT
                    intValue = int(line[0][1:])
                    data.append(packInstruction(ins[0], ins[1], ins[2], 0, 0, 0))
                    data.append(intValue)
                    return data
                case 4: # IN
                    port = int(line[0][1:])
                    dst = (int(line[1][1]) << 4) | int(line[1][-3:-1])

                    if int(dst) >> 4 == 0 and not meta[2]:
                        meta[2] = 1

                    data.append(packInstruction(ins[0], ins[1], ins[2], 0, 0, meta[2]))
                    data.append((dst << 11) | port)
                    return data
                case 5: # OUT
                    src = str((int(line[0][1]) << 4) | int(line[0][-3:-1])) if line[0][0] == '*' else line[0] # Maintains type prefix of Immediate
                    port = int(line[1][1:])

                    imme = 0
                    if scr[0] == '$':
                        imme = 1
                    if (int(src) >> 4 == 0 or (imme and int(src[1:] > 255))) and not meta[2]:
                        meta[2] = 1

                    data.append(packInstruction(ins[0], ins[1], ins[2], imme, 0, meta[2]))
                    data.append((src << 11) | port)
                    if imme:
                        data.append(int(line[2][1:]))
                    return data
                case 6 | 7: # JMP / CLL
                    addr = line[0] # Maintains type prefixes

                    imme = 0
                    if addr[0] == '$':
                        imme = 1
                        if int(addr[1:]) > 255 and not meta[2]:
                            meta[2] = 1
                    
                    data.append(packInstruction(ins[0], ins[1], ins[2], imme, 0, meta[2]))
                    data.append(int(addr[1:]))
                case _:
                    pass
        case 1: # DAT
            match ins[1]:
                case 0: # STR
                    src = str((int(line[0][1]) << 4) | int(line[0][-3:-1])) if line[0][0] == '*' else line[0] # Maintains type prefix of Immediate
                    dst = int(line[1][1:])

                    imme = 0
                    if src[0] == '$':
                        imme = 1
                    if (int(src) >> 4 == 0 or (imme and int(src[1:] > 255))) and not meta[2]:
                        meta[2] = 1

                    data.append(packInstruction(ins[0], ins[1], ins[2], imme, 0, meta[2]))
                    data.append(int(src))
                    data.append(dst)
                    return data
                case 1: # LDR
                    src = line[0] # Maintains type prefixes
                    dst = (int(line[1][1]) << 4) | int(line[1][-3:-1])

                    imme = 0
                    if src[0] == '$':
                        imme = 1
                        if int(src[1:]) > 255:
                            meta[2] = 1
                    if dst >> 4 == 0 == '0':
                        meta[2] = 1
                    
                    data.append(packInstruction(ins[0], ins[1], ins[2], imme, 0, meta[2]))
                    if imme:
                        data.append(int(dst) << 5)
                        data.append(int(src[1:]))
                    else:
                        data.append((int(dst) << 5) | int(src[1:]))
                    return data
                case 2 | 5: # MOV / SWP
                    src = (int(line[0][1]) << 4) | int(line[0][-3:-1])
                    dst = (int(line[1][1]) << 4) | int(line[1][-3:-1])

                    if (src >> 4 == 0 or dst >> 4 == 0) and not meta[2]:
                        meta[2] = 1
                    
                    data.append(packInstruction(ins[0], ins[1], ins[2], 0, 0, meta[2]))
                    data.append((dst << 5) | src)
                    return data
                case 3: # PUSH
                    src = str((int(line[0][1]) << 4) | int(line[0][-3:-1])) if line[0][0] == '*' else line[0] # Maintains type prefix of Immediate

                    imme = 0
                    if src[0] == '$':
                        imme = 1
                    if (int(src) >> 4 == 0 or (imme and int(src[1:]) > 255)) and not meta[2]:
                        meta[2] = 1
                    
                    data.append(packInstruction(ins[0], ins[1], ins[2], imme, 0, meta[2]))
                    if imme:
                        data.append(0)
                        data.append(int(src[1:]))
                    else:
                        data.append(int(src[1:]))
                        return data
                case 4: # POP
                    dst = (int(line[0][1]) << 4) | int(line[0][-3:-1])

                    if dst >> 4 == 0 and not meta[2]:
                        meta[2] = 1
                    
                    data.append(packInstruction(ins[0], ins[1], ins[2], 0, 0, meta[2]))
                    data.append((dst << 15))
                    return data
        case 2: # ARH
            match ins[1]:
                case 7: # NEG
                    main = str(int(line[0][-3:-1])) if line[0][0] == '*' else line[0] # Maintains type prefix of Immediate
                    dst = 0
                    if len(line) > 1:
                        dst = (int(line[1][1]) << 4) | int(line[1][-3:-1])
                    else:
                        dst = main

                    imme = 0
                    if main[0] == '$':
                        imme = 1
                    if int(main) >> 4 == 0 or (imme and int(src[1:]) > 255):
                        meta[2] = 1
                    
                    data.append(packInstruction(ins[0], ins[1], ins[2], imme, 0, meta[2]))
                    if imme:
                        data.append((meta[0] << 14) | (dst << 9))
                        data.append(int(main[1:]))
                    else:
                        data.append((meta[0] << 14) | (dst << 9) | int(main[1]))
                    return data
                case 8 | 9: # RSL / RSR
                    main = (int(line[0][1]) << 4) | int(line[0][-3:-1])
                    if len(line) > 1:
                        dst = (int(line[1][1]) << 4) | int(line[1][-3:-1])
                    else:
                        dst = main
                    
                    if main[0] >> 4 == 0:
                        meta[2] = 1
                    
                    data.append(packInstruction(ins[0], ins[1], ins[2], 0, 0, meta[2]))
                    data.append((meta[0] << 14) | (dst << 9) | int(main) & 0xf)
                    return data
                case 10: # CMP
                    main = str((int(line[0][1]) << 4) | int(line[0][-3:-1])) if line[0][0] == '*' else line[0] # Maintains type prefix of Immediate
                    second = str((int(line[1][1]) << 4) | int(line[1][-3:-1])) if line[1][0] == '*' else line[1] # Maintains type prefix of Immediate
                    
                    m = 0
                    s = 0
                    imme = 0
                    imme2 = 0
                    if main[0] == '$':
                        m = 0
                        imme = 1
                    else:
                        m = main
                    if (int(main) >> 4 == 0 or (imme and int(main[1:]) > 255)) and not meta[2]:
                        meta[2] = 1
                    if second[0] == '$':
                        s = 0
                        imme2 = 1
                    else:
                        s = second
                    if (second[0] == '0' or (imme2 and int(second[1:]) > 255)) and not meta[2]:
                        meta[2] = 1

                    data.append(packInstruction(ins[0], ins[1], ins[2], imme, 0, meta[2]))
                    data.append((meta[2] << 14) | (dst << 9) | ((int(s[1]) & 0xf) << 5) | (imme2 << 4) | int(m) & 0xf)
                    if imme:
                        data.append(int(main[1:]))
                    if imme2:
                        data.append(int(main[1:]))
                    return data
                case 11 | 12: # INC / DCR
                    main = (int(line[0][1]) << 4) | int(line[0][-3:-1])
                    
                    if main >> 4 == 0 and not meta[2]:
                        meta[2] = 1
                    
                    data.append(packInstruction(ins[0], ins[1], ins[2], 0, 0, meta[2]))
                    data.append(main & 0xf)
                    return data
                case _: # ADD / SUB / MUL / DIV / AND / OR / XOR
                    main = str((int(line[0][1]) << 4) | int(line[0][-3:-1])) if line[0][0] == '*' else line[0] # Maintains type prefix of Immediate
                    second = str((int(line[1][1]) << 4) | int(line[1][-3:-1])) if line[1][0] == '*' else line[1] # Maintains type prefix of Immediate
                    if len(line) > 2:
                        dst = (int(line[2][1]) << 4) | int(line[2][-3:-1])
                    else:
                        dst = main
                    
                    m = 0
                    s = 0
                    imme = 0
                    imme2 = 0
                    if main[0] == '$':
                        m = 0
                        imme = 1
                    else:
                        m = main
                    if (int(main) >> 4 == 0 or (imme and int(main[1:]) > 255)) and not meta[2]:
                        meta[2] = 1
                    if second[0] == '$':
                        s = 0
                        imme2 = 1
                    else:
                        s = second
                    if (second[0] == '0' or (imme2 and int(second[1:]) > 255)) and not meta[2]:
                        meta[2] = 1

                    data.append(packInstruction(ins[0], ins[1], ins[2], imme, meta[1], meta[2]))
                    data.append((meta[0] << 14) | (int(dst) << 9) | ((int(second) & 0xf) << 5) | (imme2 << 4) | (main & 0xf))
                    if imme:
                        data.append(main)
                    if imme2:
                        data.append(second)
                    return data

def main():
    # Take code input
    asm = []
    pAsm = []
    while True:
        line = input()
        if line.lower() == "end":
            break
        asm.append(line.lower())

    # Split into substrings of Mnemonic and Arguments

    for i in asm:
        array = i.split(' ')
        x = 0
        for i in array:
            array[x] = array[x].replace(',', '')
            x = x + 1
        pAsm.append(array)

    tokenised = []

    for line in pAsm:
        tokenised.append(tokenise(line))

    for x in range(3):
        for a, line in enumerate(tokenised):
            tokenised[a] = translateLabels(a, line)

    for a, line in enumerate(tokenised):
        print(line)
        checkInstructions(a, line)

    tokenised = [x for x in tokenised if x]

    print(tokenised)
    print(miscTokens)

    data = []

    for line in tokenised:
        data.append(assemble(line))

    print(data)
    print(miscTokens)

    with open('out.bin', 'wb') as file:
        for line in data:
            for binary in line:
                width = 1
                bitLength = binary.bit_length()
                if bitLength > 8:
                    width = 2
                file.write((binary).to_bytes(width, byteorder='little', signed=False))

main()