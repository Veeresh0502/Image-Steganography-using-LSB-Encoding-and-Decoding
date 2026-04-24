# Image Steganography using LSB Encoding & Decoding

![Language](https://img.shields.io/badge/Language-Embedded%20C-blue)
![Topic](https://img.shields.io/badge/Topic-Steganography-purple)
![Format](https://img.shields.io/badge/Format-BMP%20Images-orange)
![License](https://img.shields.io/badge/License-Open%20Source%20%28Educational%29-green)

> Hide secret text inside BMP images — invisibly. Extract it back without losing a single bit.

---

## 📌 Overview

This project implements **LSB (Least Significant Bit) steganography** in C. Secret text is embedded directly into the pixel data of a BMP image by replacing the least significant bits of each byte — leaving the image visually unchanged while carrying hidden information.

| Phase | What it does |
|---|---|
| **Encode** | Reads a `.bmp` image + secret text → writes a stego BMP |
| **Decode** | Reads the stego BMP → extracts and prints the hidden text |

---

## ⚙️ How LSB Encoding Works

Each pixel channel (R, G, B) in a BMP stores 8 bits. Replacing only the **last 1–2 bits** causes a color change too small for the human eye to detect.

```
Original byte:  1 1 0 1 0 1 1 0
Secret bit:                     1
Result:         1 1 0 1 0 1 1 1   ← visually identical
```

The message length is also encoded in the header region so the decoder knows exactly how many bits to extract.

---

## 📂 Project Structure

```
.
├── encode.c          ← encoding logic
├── encode.h          ← encoder declarations
├── decode.c          ← decoding logic
├── decode.h          ← decoder declarations
├── common.h          ← shared structs and constants
├── types.h           ← type definitions
├── test_encode.c     ← entry point / CLI handler
├── makefile          ← build system
├── beautiful.bmp     ← sample cover image
├── secret.txt        ← sample secret message
└── stego.bmp         ← output image with hidden data
```

---

## ▶️ Getting Started

### Build

```bash
make
```

### Encode — hide a secret message

```bash
./a.out -e beautiful.bmp secret.txt stego.bmp
```

### Decode — extract the hidden message

```bash
./a.out -d stego.bmp output_stego.txt
```

---

## 🖼️ Example

**Encode**

```
[INFO] Opening files...
[INFO] Encoding secret message length...
[INFO] Encoding secret message data...
[INFO] Encoding complete. Stego image saved as stego.bmp
```

**Decode**

```
[INFO] Decoding hidden message...
[INFO] Decoded message saved to output_stego.txt
```

After decoding, `output_stego.txt` contains the exact original message. The two BMP files are visually identical.

---

## 🛠️ Technologies Used

- Embedded C
- File I/O (BMP binary read/write)
- Bitwise operations (`&`, `|`, `<<`, `>>`)
- Pointers & memory manipulation
- Structs for BMP header parsing
- Makefile-based build system
- Command line arguments (`argc`, `argv`)

---

## 🧠 Key Learnings

- Understood BMP file structure — file header, info header, and raw pixel data layout
- Implemented precise bitwise encoding and decoding without data loss
- Maintained full image integrity while embedding hidden data
- Handled binary file I/O carefully to avoid byte misalignment

---

## ⚠️ Challenges Faced

- Parsing BMP headers correctly to locate the pixel data offset
- Encoding message length so the decoder knows exactly when to stop
- Avoiding pixel overflow when replacing LSBs near byte boundaries
- Keeping the image visually lossless across different BMP inputs

---

## 🚀 Roadmap

- [ ] Support PNG and JPEG formats
- [ ] Add password-protected encryption before embedding
- [ ] Support embedding binary files, not just text
- [ ] GUI-based drag-and-drop interface
- [ ] Steganalysis mode — detect hidden data in suspicious images

---

## 👨‍💻 Author

**Veeresh T** (`Veeresh0502`)

Developed as part of learning System Programming and Embedded C concepts.

---

## ⭐ License

Open source for educational use. Fork it, learn from it, build on it!
