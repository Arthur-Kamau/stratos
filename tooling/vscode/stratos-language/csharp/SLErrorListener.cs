using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;
using System.IO;

namespace AntlrServer
{
    public class SLErrorListener: BaseErrorListener
    {
        public List<EditorMessage> ErrorMessages {get; private set;} // Сообщения об ошибке
        public SLErrorListener()
        {
            ErrorMessages = new List<EditorMessage>();
        }

        public override void SyntaxError(IRecognizer recognizer, IToken offendingSymbol, int line, int charPositionInLine, string msg, RecognitionException e)
        {
            // Аналогично лексеру -- перегрузка соответствующего метода
            EditorMessage em = new EditorMessage()
            {
                Message = msg,
                Line = line - 1,
                Character = charPositionInLine,
                Symbol = "",
                Length = 0
            };

            if(offendingSymbol != null)
            {
                em.Symbol = offendingSymbol.Text;
                em.Length = offendingSymbol.StopIndex - offendingSymbol.StartIndex + 1;
            }

            ErrorMessages.Add(em);  
        }
    }
}