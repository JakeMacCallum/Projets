
from winsound import Beep
from time import sleep

MORSE_CODE_DICT = {
    'A': '=.===',
    'B': '===.=.=.=',
    'C': '===.=.===.=',
    'D': '===.=.=',
    'E': '=',
    'F': '=.=.===.=',
    'G': '===.===.=',
    'H': '=.=.=.=',
    'I': '=.=',
    'J': '=.===.===.===',
    'K': '===.=.===',
    'L': '=.===.=.=',
    'M': '===.===',
    'N': '===.=',
    'O': '===.===.===',
    'P': '=.===.===.=',
    'Q': '===.===.=.===',
    'R': '=.===.=',
    'S': '=.=.=',
    'T': '===',
    'U': '=.=.===',
    'V': '=.=.=.===',
    'W': '=.===.===',
    'X': '===.=.=.===',
    'Y': '===.=.===.===',
    'Z': '===.===.=.=',
    '1': '=.===.===.===.===',
    '2': '=.=.===.===.===',
    '3': '=.=.=.===.===',
    '4': '=.=.=.=.===',
    '5': '=.=.=.=.=',
    '6': '===.=.=.=.=',
    '7': '===.===.=.=.=',
    '8': '===.===.===.=.=',
    '9': '===.===.===.===.=',
    '0': '===.===.===.===.===',
    ' ': '.'
}


def francais_morse(chaine_francais):
    chaine_morse = ""
    for i in chaine_francais:
        chaine_morse += MORSE_CODE_DICT[i]+"..."
    return chaine_morse[:-3]


def morse_francais(chaine_morse):
    chaine_francais = ""
    indice1 = 0
    for i in range(len(chaine_morse)+1):
        if chaine_morse[i:i+3] == "..." or i == len(chaine_morse):
            for francais, morse in MORSE_CODE_DICT.items():
                if chaine_morse[indice1:i] == morse:
                    chaine_francais += francais
            if chaine_morse[i:i+7] == ".......":
                indice1 = i + 7
                chaine_francais += " "
            else:
                indice1 = i + 3
    return chaine_francais


def beep_morse(chaine_morse):
    i = 0
    while i < len(chaine_morse):
        if chaine_morse[i] == ".":
            if chaine_morse[i:i+7] == ".......":
                sleep(2.1)
                i += 7
            elif chaine_morse[i:i+3] == "...":
                sleep(0.9)
                i += 3
            else:
                sleep(0.3)
                i += 1
        elif chaine_morse[i:i+3] == "===":
            Beep(500, 800)
            i += 3
        else:
            Beep(500, 250)
            i += 1
beep_morse(francais_morse("SOS"))