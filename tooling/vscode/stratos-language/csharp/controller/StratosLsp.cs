using System;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Routing;

namespace StratosServer {
    public class StratosServerRouter: IRouter {
        public Task RouteAsync(RouteContext context) {
            var number = context.RouteData.Values["number"] as string;
            if (string.IsNullOrEmpty(number)) {
                return Task.FromResult(0);
            }

            int value;
            if (!Int32.TryParse(number, out value)) {
                return Task.FromResult(0);
            }

            var requestPath = context.HttpContext.Request.Path;
            if (requestPath.StartsWithSegments("/fib", StringComparison.OrdinalIgnoreCase)) {
                context.Handler = async c => {
                    //var answer = Fibonacci.FindNumber(value);
                    // await c.Response.WriteAsync($"Fib({number}) = {answer}");
                    await c.Response.WriteAsync($"Fib  ........ ");
                };
            }
            return Task.FromResult(0);
        }

        public VirtualPathData GetVirtualPath(VirtualPathContext context) {
            return null;
        }
    }
}