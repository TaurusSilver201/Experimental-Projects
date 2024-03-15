from googletrans import Translator, constants
import tkinter as tk
from tkinter import ttk, scrolledtext, filedialog, messagebox
import threading
import tkinter.font as tkfont

# Initialize the Translator object
translator = Translator()

# List of supported languages
language_list = constants.LANGUAGES.values()

# Function to translate text
def translate_text(src_lang, dest_lang):
    text = input_text.get("1.0", "end-1c")
    if text:
        output_text.config(state="normal")
        output_text.delete("1.0", "end")
        output_text.insert("end", "Translating...")
        translate_button.config(state="disabled")
        open_button.config(state="disabled")
        save_button.config(state="disabled")

        # Perform translation in a separate thread
        threading.Thread(target=translate_worker, args=(text, src_lang, dest_lang)).start()
    else:
        output_text.config(state="normal")
        output_text.delete("1.0", "end")
        output_text.insert("end", "Please enter some text to translate.")

# Worker function to handle translation
def translate_worker(text, src_lang, dest_lang):
    try:
        translation = translator.translate(text, src=src_lang, dest=dest_lang)
        output_text.config(state="normal")
        output_text.delete("1.0", "end")
        output_text.insert("end", translation.text)
    except Exception as e:
        output_text.config(state="normal")
        output_text.delete("1.0", "end")
        output_text.insert("end", f"Error: {str(e)}")
    finally:
        translate_button.config(state="normal")
        open_button.config(state="normal")
        save_button.config(state="normal")
        output_text.config(state="disabled")

# Function to open a file
def open_file():
    file_path = filedialog.askopenfilename()
    if file_path:
        try:
            with open(file_path, "r", encoding="utf-8") as file:
                content = file.read()
                input_text.delete("1.0", "end")
                input_text.insert("end", content)
        except Exception as e:
            messagebox.showerror("Error", str(e))

# Function to save the translated text
def save_file():
    translated_text = output_text.get("1.0", "end-1c")
    if translated_text:
        file_path = filedialog.asksaveasfilename(defaultextension=".txt")
        if file_path:
            try:
                with open(file_path, "w", encoding="utf-8") as file:
                    file.write(translated_text)
            except Exception as e:
                messagebox.showerror("Error", str(e))
    else:
        messagebox.showwarning("Warning", "No translated text to save.")

# Create the main window
window = tk.Tk()
window.title("Text Translator")

# Create labels
input_label = ttk.Label(window, text="Enter text:")
input_label.grid(row=0, column=0, padx=5, pady=5, sticky="w")

output_label = ttk.Label(window, text="Translated text:")
output_label.grid(row=0, column=1, padx=5, pady=5, sticky="w")

# Create text boxes
input_text = scrolledtext.ScrolledText(window, width=40, height=10)
input_text.grid(row=1, column=0, padx=5, pady=5)

output_text = scrolledtext.ScrolledText(window, width=40, height=10, state="disabled")
output_text.grid(row=1, column=1, padx=5, pady=5)

# Create dropdown menus for language selection
src_lang_label = ttk.Label(window, text="Source Language:")
src_lang_label.grid(row=2, column=0, padx=5, pady=5, sticky="w")

src_lang_var = tk.StringVar()
src_lang_dropdown = ttk.Combobox(window, textvariable=src_lang_var, values=language_list, state="readonly")
src_lang_dropdown.current(list(language_list).index("Japanese"))
src_lang_dropdown.grid(row=2, column=0, padx=5, pady=5, sticky="e")

dest_lang_label = ttk.Label(window, text="Destination Language:")
dest_lang_label.grid(row=2, column=1, padx=5, pady=5, sticky="w")

dest_lang_var = tk.StringVar()
dest_lang_dropdown = ttk.Combobox(window, textvariable=dest_lang_var, values=language_list, state="readonly")
dest_lang_dropdown.current(list(language_list).index("Bengali"))
dest_lang_dropdown.grid(row=2, column=1, padx=5, pady=5, sticky="e")

# Create buttons
translate_button = ttk.Button(window, text="Translate", command=lambda: translate_text(src_lang_var.get(), dest_lang_var.get()))
translate_button.grid(row=3, column=0, columnspan=1, padx=5, pady=5)

open_button = ttk.Button(window, text="Open File", command=open_file)
open_button.grid(row=3, column=1, columnspan=1, padx=5, pady=5)

save_button = ttk.Button(window, text="Save Translation", command=save_file)
save_button.grid(row=4, column=0, columnspan=2, padx=5, pady=5)

# Increase the font size
default_font = tkfont.nametofont("TkDefaultFont")
default_font.configure(size=12)

# Start the main event loop
window.mainloop()
