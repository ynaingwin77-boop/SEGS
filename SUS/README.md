# Student Utility System (SUS) — Colorful Version

This version keeps the original functionality and adds:
- 📚 📖 📋 📊 👨‍🎓 emojis for quick visual identification
- ANSI terminal colors for menus and section headers
- Colored success/update/delete/status messages
- Original CSV data included

## Run
From the `SUS` folder:

```bash
./sus
```

If macOS/Linux says permission denied:

```bash
chmod +x sus
./sus
```

## Make changes
Edit the `.c` files and/or CSV files in `data/`, then recompile:

```bash
gcc main.c students.c books.c issues.c attendance.c reports.c utils.c -I./SrcHeaders -o sus
```

The program is intended for a terminal that supports UTF-8 emojis and ANSI colors.
