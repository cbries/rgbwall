using System;

namespace Tetris
{
    public class DrawConsole : IDraw
    {
        public void Render(IGame game)
        {
            if (game == null)
                return;

            Console.WriteLine("*** Render() ***");

            int w = game.Width;
            int h = game.Height;

            for (int x = 0; x < w; ++x)
            {
                for (int y = 0; y < h; ++y)
                {
                    var px = game.GetPixel(x, y);

                    if (!px.HasValue())
                    {
                        Console.Write("   ");

                        continue;
                    }

                    Console.Write(" x ");
                }

                Console.WriteLine("");
            }
        }
    }
}
