# Linux Device Driver: Caesar Cipher (Encryption/Decryption)

This project implements a Linux character device driver in C that performs text encryption and decryption using a Caesar cipher. The driver exposes a custom device file (`/dev/secret`) that allows user-space programs to interact with the kernel module.

## 🚀 Features

- Character device driver implemented in Linux kernel space
- Supports standard file operations: `open`, `read`, `write`, and `release`
- Uses `ioctl` for device control
- Dynamic Caesar cipher key (user-defined at runtime)
- Interactive user-space test program
- Safe data transfer between user space and kernel space (`copy_to_user`, `copy_from_user`)

## 🧠 How It Works

1. The user program writes a string to the device.
2. The driver stores the string in a kernel buffer.
3. The user selects an operation via `ioctl`:
   - `'e'` → Encrypt
   - `'d'` → Decrypt
   - `'k'` → Set encryption key
4. The driver processes the string using a Caesar cipher.
5. The processed result is returned to the user via `read`.

## 🛠️ Technologies Used

- C Programming (Kernel + User Space)
- Linux Kernel Modules
- Device Driver Development
- System Calls (`open`, `read`, `write`, `ioctl`)
- Memory Management (`vmalloc`, `vfree`)

## 🧪 Example
