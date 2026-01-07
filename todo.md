 great job i was looking into documeentation generation in
C:\Users\ADMIN\Desktop\Development\Projects\stratos\examples\comments-and-documentation and got this

  After checking docs-md/math_utils.md, I see that the detailed documentation within functions (e.g.,
  descriptions, @param, @return, @example tags) is still not being extracted and rendered in the generated output.

  This points to a deeper issue within the stratos doc generate tool's internal documentation parsing logic
  (specifically in the Parser::parseDocComment function, or how the extracted DocComment object is utilized by the
  MarkdownDocGenerator). While the comments are being identified, their detailed content (summary, description,
tags) is
  not being correctly processed for display.

  To fully resolve this, further debugging and modifications to the Stratos documentation generator's C++ source
code
  would be required, followed by a recompilation of the stratos.exe binary.