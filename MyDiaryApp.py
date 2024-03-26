import os
import tkinter as tk
from tkinter import filedialog, scrolledtext, messagebox, font
from datetime import date, datetime, timedelta
import pickle
import calendar
import tkinter.ttk as ttk
import shutil

def get_diary_entry(master):
    """Open a GUI window to allow the user to enter their diary entry."""
    window = tk.Toplevel(master)
    window.title("Diary Entry")
    text_area = scrolledtext.ScrolledText(window, width=60, height=20, font=("Arial", 12))
    text_area.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
    button_frame = tk.Frame(window)
    button_frame.pack(pady=10)

    def save_and_close():
        entry = text_area.get("1.0", tk.END).strip()
        if entry:
            save_diary_entry(entry, selected_date)
        window.destroy()

    save_button = tk.Button(button_frame, text="Save", command=save_and_close, font=("Arial", 12))
    save_button.pack(side=tk.LEFT, padx=5)
    cancel_button = tk.Button(button_frame, text="Cancel", command=window.destroy, font=("Arial", 12))
    cancel_button.pack(side=tk.LEFT, padx=5)

def save_diary_entry(entry, date_str):
    """Save the diary entry to a file in the appropriate folder."""
    folder_path = os.path.join("diary", date_str[:7])  # Save entries in YYYY-MM folders
    os.makedirs(folder_path, exist_ok=True)
    file_path = os.path.join(folder_path, f"{date_str}.dat")
    with open(file_path, "wb") as file:
        pickle.dump(entry, file)
    print(f"Diary entry saved to {file_path}")

def load_diary_entry(date_str):
    """Load and display the diary entry for the given date."""
    folder_path = os.path.join("diary", date_str[:7])
    file_path = os.path.join(folder_path, f"{date_str}.dat")
    if os.path.exists(file_path):
        with open(file_path, "rb") as file:
            content = pickle.load(file)
        text_area.delete("1.0", tk.END)
        text_area.insert(tk.END, content)
    else:
        text_area.delete("1.0", tk.END)

def backup_diary():
    """Create a backup of the diary folder."""
    backup_folder = filedialog.askdirectory(title="Select Backup Folder", initialdir=os.getcwd())
    if backup_folder:
        backup_path = os.path.join(backup_folder, "diary_backup")
        try:
            shutil.copytree("diary", backup_path)
            messagebox.showinfo("Backup Successful", f"Diary has been backed up to {backup_path}")
        except Exception as e:
            messagebox.showerror("Backup Failed", str(e))

def main():
    root = tk.Tk()
    root.title("Diary")
    root.geometry("800x600")

    menu_bar = tk.Menu(root)
    root.config(menu=menu_bar)
    file_menu = tk.Menu(menu_bar)
    menu_bar.add_cascade(label="File", menu=file_menu)
    file_menu.add_command(label="New Entry", command=lambda: get_diary_entry(root))
    file_menu.add_command(label="Backup Diary", command=backup_diary)
    file_menu.add_separator()
    file_menu.add_command(label="Exit", command=root.quit)

    date_frame = tk.Frame(root)
    date_frame.pack(pady=10)

    def previous_date():
        nonlocal selected_date
        selected_date = (datetime.strptime(selected_date, "%Y-%m-%d") - timedelta(days=1)).strftime("%Y-%m-%d")
        update_date_label()
        load_diary_entry(selected_date)

    def next_date():
        nonlocal selected_date
        selected_date = (datetime.strptime(selected_date, "%Y-%m-%d") + timedelta(days=1)).strftime("%Y-%m-%d")
        update_date_label()
        load_diary_entry(selected_date)

    def update_date_label():
        date_label.config(text=selected_date)

    def select_date():
        nonlocal selected_date
        selected_date = cal.selection_get().strftime("%Y-%m-%d")
        update_date_label()
        load_diary_entry(selected_date)

    prev_button = tk.Button(date_frame, text="<", command=previous_date, font=("Arial", 12))
    prev_button.pack(side=tk.LEFT, padx=5)

    selected_date = date.today().strftime("%Y-%m-%d")
    date_label = tk.Label(date_frame, text=selected_date, font=("Arial", 14, "bold"))
    date_label.pack(side=tk.LEFT)

    next_button = tk.Button(date_frame, text=">", command=next_date, font=("Arial", 12))
    next_button.pack(side=tk.LEFT, padx=5)

    cal_frame = tk.Frame(root)
    cal_frame.pack(pady=10)
    cal = calendar.Calendar(cal_frame, selectmode="day", year=int(selected_date[:4]), month=int(selected_date[5:7]), day=int(selected_date[8:]))
    cal.pack()
    cal.bind("<<CalendarMonthChanged>>", lambda event: cal.selection_set(selected_date))
    cal.bind("<<CalendarDaySelected>>", lambda event: select_date())

    text_area = scrolledtext.ScrolledText(root, width=60, height=20, font=("Arial", 12))
    text_area.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

    load_diary_entry(selected_date)

    root.mainloop()

if __name__ == "__main__":
    main()
