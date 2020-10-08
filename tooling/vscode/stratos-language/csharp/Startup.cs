using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
// using Antlr4.Runtime;
// using Antlr4.Runtime.Misc;
// using Antlr4.Runtime.Tree;
using System.IO;
// using static SemanticLanguageGrammar.SLGrammarParser;
using System.Text;
using Microsoft.AspNetCore.Routing;
// using SemanticLanguageGrammar;

namespace StratosServer
{
    public class Startup
    {
        // This method gets called by the runtime. Use this method to add services to the container.
        // For more information on how to configure your application, visit https://go.microsoft.com/fwlink/?LinkID=398940
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddRouting();
        }

        public void Configure(IApplicationBuilder app, IHostingEnvironment env)
        {
            
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }

            var rb = new RouteBuilder(app);

            RequestDelegate factorialRequestHandler =  c =>
            {
                
                return  c.Response.WriteAsync(" Message JSON  .");
            };

            rb.MapPost("parse", factorialRequestHandler);

            var routes = rb.Build();

            app.UseRouter(routes);
        }


        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        // public void Configure(IApplicationBuilder app, IHostingEnvironment env, ILoggerFactory loggerFactory)
        // {
        //     loggerFactory.AddConsole();

        //     if (env.IsDevelopment())
        //     {
        //         app.UseDeveloperExceptionPage();
        //     }

        //     var routeBuilder = new RouteBuilder(app);                

        //     routeBuilder.MapPost("parse", context =>
        //     {                
        // string text = "";
        // using(StreamReader reader = new StreamReader(context.Request.Body))
        // {
        //     text = reader.ReadToEnd();
        // }

        //         AntlrInputStream inputStream = new AntlrInputStream(text);
        //         SLGrammarLexer lexer = new SLGrammarLexer(inputStream);
        //         CommonTokenStream commonTokenStream = new CommonTokenStream(lexer);
        //         SLGrammarParser parser = new SLGrammarParser(commonTokenStream);            

        //         SLErrorListener errorListener = new SLErrorListener();
        //         SLLexerErrorListener lexerErrorListener = new SLLexerErrorListener();

        //         lexer.RemoveErrorListeners();
        //         lexer.AddErrorListener(lexerErrorListener);   
        //         parser.RemoveErrorListeners();
        //         parser.AddErrorListener(errorListener);

        // var g = parser.start();            

        //         StringBuilder json = new StringBuilder();
        //         json.Append("{");                
        //         json.Append("\"errors\": [");

        //         json.Append(convertMessagesToJson(lexerErrorListener.ErrorMessages));
        //         json.Append(convertMessagesToJson(errorListener.ErrorMessages));

        //         if(lexerErrorListener.ErrorMessages.Count + errorListener.ErrorMessages.Count > 0)
        //             json.Remove(json.Length-2, 1);
        // json.Append("]");
        // json.Append("}");

        //         Console.WriteLine(json.ToString());
        //         return context.Response.WriteAsync(json.ToString());
        //         return context.Response.WriteAsync("[\"message\":\"Hey there\"]");
        //     });

        //     var routes = routeBuilder.Build();

        //     app.UseRouter(routes);
        // }

        // public string convertMessagesToJson(List<EditorMessage> messages)
        // {
        //     StringBuilder json = new StringBuilder();

        //     foreach(var message in messages)
        //     {
        //         json.Append("{ ");
        //         json.Append($"\"message\" : \"{message.Message}\", ");
        //         json.Append($"\"line\" : {message.Line}, ");
        //         json.Append($"\"character\" : {message.Character}, ");
        //         // yes, we don't actually use it in the server, but it could be useful
        //         // for instance to find all errors relative to a symbol
        //         json.Append($"\"symbol\" : \"{message.Symbol}\", ");
        //         json.Append($"\"length\" : {message.Length}");
        //         json.Append("}, ");
        //     }                        

        //     return json.ToString();
        // }

    }
}
