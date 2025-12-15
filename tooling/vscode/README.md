# stratos-laguage README

The current version is limited to a simple syntax highlighting grammar, Code snippets and simple commands.
Future work is to include the type checker, smart code completion, a debugger, and ideally, deeper symbolic analyzers.


Run `npm install` and `npm run compile`, your VS Code extension is built.

  To test and use the extension:

   1. Open the Extension in VS Code:
       * Open VS Code.
       * Go to File > Open Folder... and select the folder C:\Users\ADMIN\Desktop\Development\Projects\stratos\tooling\vscode.

   2. Launch the Extension Development Host:
       * Once the folder is open, press F5.
       * This will open a new VS Code window, called the "Extension Development Host". This is a clean instance of VS Code where your Stratos extension is running.

   3. Test the Extension:
       * In the "Extension Development Host" window, open any of your .st Stratos files (e.g., from C:\Users\ADMIN\Desktop\Development\Projects\stratos\compiler\cases).
       * You should now see:
           * Syntax Highlighting for Stratos keywords, types, and operators.
           * Code Completion when you start typing keywords like val, var, fn, class, Optional.
           * Diagnostics (e.g., warnings for all-caps words, or errors if you introduce syntax mistakes).