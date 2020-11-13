package com.stratos.old.Syntax;

import com.stratos.model.Node;
import com.stratos.model.NodeType;
import com.stratos.old.Statement.ImportStatement;
import com.stratos.old.Statement.Statement;
import com.stratos.model.Token;
import com.stratos.util.Print.Log;

public class ImportSyntax extends Syntax {


    public Statement execute(Token token) {
        super.execute(token.getNodes());

        Statement statement = null;
        while (!super.isAtEnd()) {
            Node c = super.advance();
            if (c.getType() == NodeType.ImportNode ) {

                Node importPath = super.advance();

                if (importPath.getType() == NodeType.AlphaNumericNode ) {

                    statement = (new ImportStatement(
                            importPath,
                            c
                    ));
                }else  if( importPath.getType() == NodeType.StringValue){
                    statement = (new ImportStatement(
                            importPath,
                            c
                    ));
                } else {
                    Log.error("No alpanumeric " + peek().toString());
                }

            }
        }

        return statement;
    }
}
