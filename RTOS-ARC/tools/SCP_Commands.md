# SCP Commands (Windows ↔ ARC)

All commands below are run from **Windows Command Prompt or PowerShell**.
ARC cannot push files to your laptop directly, so you must run `scp` from Windows.

---

# Windows → ARC (Upload)

## Copy a folder from Windows to ARC

### Syntax

```bash
scp -r "<Windows-Folder-Path>" <unityID>@arc-node:/home/<unityID>/
```

### Example

```bash
scp -r "C:\Users\consa\Downloads\Academics\NC_STATE\2025-2026\SPRING_2026\RTOS_ML\RTOS-ARC" cbsavugo@arc-node:/home/cbsavugo/
```

Result: The folder is uploaded to your ARC home directory.

---

# ARC → Windows (Download)

## Copy a folder from ARC to Windows

### Syntax

```bash
scp -r <unityID>@arc-node:/home/<unityID>/<folder> "<Windows-Destination-Folder>"
```

### Example

```bash
scp -r cbsavugo@arc-node:/home/cbsavugo/RTOS-ARC "C:\Users\consa\Downloads\Academics\NC_STATE\2025-2026\SPRING_2026\RTOS_ML"
```

Result: The folder is downloaded to your Windows directory.

---

# Single File Transfers

## Upload one file

```bash
scp "C:\Users\consa\Downloads\file.txt" cbsavugo@arc-node:/home/cbsavugo/
```

## Download one file

```bash
scp cbsavugo@arc-node:/home/cbsavugo/file.txt "C:\Users\consa\Downloads"
```

---

# Notes

* Always wrap Windows paths in quotes because of spaces.
* `arc-node` must exist in your SSH config (created by the set-arc-node script).
* These commands are run from Windows, not from ARC.
