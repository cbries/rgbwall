using System;

namespace Tetris
{
    public class DrawConsole : IDraw
    {
        private int _renderCounter = 0;

        public void Render(IGame game)
        {
            if (game == null)
                return;

            ++_renderCounter;

            Console.Clear();
            Console.WriteLine("*** Render() *** {0}", _renderCounter);

            int w = game.Width;
            int h = game.Height;

            for (int x = 0; x < w; ++x)
            {
                for (int y = 0; y < h; ++y)
                {
                    var px = game.GetPixel(x, y);

                    if (px == null)
                        continue;

                    if (!px.HasValue())
                    {
                        Console.Write("   ");

                        continue;
                    }

                    Console.Write(" x ");
                }

                //var block = game.CurrentBlock;
                //if (block != null)
                //{
                //    var blockPxls = block.Pixels;
                //    if (blockPxls != null)
                //    {
                //        var blockX = block.X;
                //        var blockY = block.Y;

                //        foreach (var px in blockPxls)
                //        {
                //            if (px == null)
                //                continue;

                //            var bx = px.X + blockX;
                //            var by = px.Y + blockY;

                //            Con
                //        }
                //    }
                //}

                Console.WriteLine("");
            }
        }
    }
}
