package model.Statement;

import model.StatementType;

import java.util.List;

public class CallStatement extends Statement {
   String Identifier;
   List<ArgumentStatements> argumentStatementsList ;

   public CallStatement(StatementType statementType) {
      super(statementType);
   }
}
