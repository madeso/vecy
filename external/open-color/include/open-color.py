#!/usr/bin/env python3

import collections.abc
import json


def is_array(a):
    return type(a) in [list]

def to_hex(str):
    s = str[1:]
    return f'0x{s}'

def print_array(name, colors):
    nl = []

    for i, c in enumerate(colors):
        cn = f'{name}_{i}'
        nl.append(cn)
        print(f'constexpr Hex {cn} = {to_hex(c)};')

    nl = ', '.join(nl)
    print(f'constexpr Hexs {name} = {{{nl}}};')
    print()

def print_single(name, color):
    print(f'constexpr Hex {name} = {to_hex(color)};')
    print()

def main():
    data = json.load(open('open-color.json'))

    for name, colors in data.items():
        if is_array(colors):
            print_array(name, colors)
        else:
            print_single(name, colors)

main()