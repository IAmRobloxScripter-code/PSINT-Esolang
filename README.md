# Psint(Please Speed i need this!) — A Joke Esolang  
**Created by Scripting_Entity**  

Psint is a small esoteric language with **three unsigned 8-bit registers**: `mommas`, `dad`, and `speed`. You manipulate these registers with simple instructions to create programs.  

---

## Registers

| Register  | Description                                |
|-----------|--------------------------------------------|
| `mommas`  | First general-purpose register             |
| `dad`     | Second general-purpose register            |
| `speed`   | Third general-purpose register             |

> mommas register is the selected register by default.
---

## Instructions

| Instruction                     | Description |
|---------------------------------|-------------|
| `please <register>`             | Selects a register to operate on. `<register>` can be `mommas`, `dad`, or `speed`. Example: `please mommas` selects the `mommas` register. |
| `need { ... }`                | Loop instruction. Repeats the block `{ ... }` until the currently selected register is 0. Example: `need { live }` adds 1 to the selected register until the selected register is 0. |
| `live`                          | Increment the currently selected register by 1. |
| `kinda`                         | Decrement the currently selected register by 1. |
| `with`                          | Rotate registers forward. Example: if registers are `(mommas=1, dad=100, speed=67)`, after `with` they become `(mommas=67, dad=1, speed=100)`. |
| `help`                          | Rotate registers backward (opposite of `with`). |
| `my`                            | Reads a single character from user input into the currently selected register. |
| `homeless`                       | Prints the ASCII character corresponding to the value in the currently selected register. |

---

## Examples
**"Hello World!" in psint:**
```psint 
kinda
need {
  kinda kinda kinda kinda kinda 
  please dad
  live live live live live live
  please speed live 
  please mommas
}

please dad
need {
  kinda kinda kinda kinda kinda
  please speed
  live live
  please dad
}

please speed live
homeless

please dad live live live live live live live live live live
need {
  kinda
  please speed live live live
  please dad
}
please speed
kinda
homeless
live live live live live live live
homeless homeless live live live homeless

need {
    kinda
    please dad live
    please mommas live
    please speed
}
please mommas
kinda
need {
  kinda kinda
  please dad
  kinda
  please mommas
}
please dad
please mommas
kinda
kinda kinda kinda kinda kinda
kinda kinda kinda kinda kinda
kinda kinda kinda kinda kinda
need {
  kinda kinda kinda kinda
  kinda kinda kinda kinda
  kinda kinda kinda kinda
  please dad kinda
  please mommas
}
please dad kinda kinda kinda kinda
homeless

need {
    kinda
    please mommas live
    please speed live
    please dad
}

please mommas
need {
    kinda kinda 
    please speed live live live
    please mommas
}
please speed live live live live live live live
homeless

need {
    kinda
    please mommas live
    please dad live
    please speed
}
please mommas
kinda
need {
  kinda kinda
  please dad live
  please mommas
}
please dad
need {
    kinda kinda
    please speed live
    please dad
}
please speed
kinda kinda kinda kinda kinda
need {
    kinda
    please dad live live
    please speed
}
please dad kinda kinda kinda kinda kinda kinda kinda kinda kinda
homeless live live live homeless kinda kinda kinda kinda kinda kinda homeless
kinda kinda kinda kinda kinda kinda kinda kinda homeless

need {
    kinda
    please speed live
    please mommas live
    please dad 
}

please mommas
need {
    kinda kinda kinda kinda
    please speed kinda kinda kinda
    please mommas
}
please speed live live live live live live live live
homeless
```

## How to compile?
**GCC:** `gcc -O3 -s esolang.c -o psint`
**CLANG:** `clang -O3 -s esolang.c -o psint`
