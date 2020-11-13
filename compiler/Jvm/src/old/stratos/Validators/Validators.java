package old.stratos.Validators;

import old.stratos.model.Statement.Statement;
import old.stratos.model.Diagnostics;

import java.util.ArrayList;
import java.util.List;

/**
 * Validate expressions
 * Validations include
 *  1. Ensure unique function names in a package.
 *  2. Ensure file path and package name match.
 *  3. ensure proper assignment to variables
 *  4. ensure proper variable declaration and if another variable exists of the same name.
 *
 */
public class Validators {

    List<Diagnostics> diagnostics = new ArrayList<>();
    public List<Statement>  validate(List<Statement> statements){


        return  statements;
    }

    public List<Diagnostics> getDiagnostics() {
        return diagnostics;
    }
}

