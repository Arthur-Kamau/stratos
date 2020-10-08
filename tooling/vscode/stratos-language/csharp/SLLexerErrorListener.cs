using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Antlr4.Runtime;
using System.IO;

namespace AntlrServer
{
    public class SLLexerErrorListener: IAntlrErrorListener<int>
    {
        public List<EditorMessage> ErrorMessages {get; private set;} // Сообщения об ошибке
        public SLLexerErrorListener()
        {
            ErrorMessages = new List<EditorMessage>();
        }

        public void SyntaxError(IRecognizer recognizer, int offendingSymbol, int line, int charPositionInLine, string msg, RecognitionException e)
        {
            // Перегрузка метода SyntaxError -- сохранение полей в отдельную структуру
            EditorMessage em = new EditorMessage()
            {
                Message = msg,
                Line = line - 1,
                Character = charPositionInLine,
                Symbol = "",
                Length = 1
            };

            if (offendingSymbol != 0) // Для нормального отображения токенов 
            {
                em.Symbol = recognizer.Vocabulary.GetDisplayName(offendingSymbol);
            }
            ErrorMessages.Add(em);
        }
    }
}