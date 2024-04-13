import os

file_path = r"CV\\best.pt"

if os.path.exists(file_path):
    print(f"File {file_path} ditemukan.")
else:
    print(f"File {file_path} tidak ditemukan.")
