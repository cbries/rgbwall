using System;
using System.Collections.Generic;

namespace Tetris
{
    public class Block
    {
        private static Random _rnd = new Random(DateTime.Now.Millisecond);

        public Pixel[] Pixels { get; protected set; }

        public Pixel[] RealPixels
        {
            get
            {
                var pxs = new List<Pixel>();
                for (int i = 0; i < 4; ++i)
                {
                    var idx = i + _rotIndex * 4;
                    pxs.Add(Pixels[idx]);
                }
                return pxs.ToArray();
            }
        }

        public Block()
        {
            X = 4;
            Y = 0;
        }

        public static Block NextBlock()
        {
            int idx = _rnd.Next(0, 7);
            switch (idx)
            {
                case 0: return new BlockI();
                case 1: return new BlockJ();
                case 2: return new BlockL();
                case 3: return new BlockO();
                case 4: return new BlockS();
                case 5: return new BlockT();
                case 6: return new BlockZ();
            }

            return null;
        }

        public int X { get; protected set; }
        public int Y { get; protected set; }

        private int _rotIndex = 0;

        public void RotCcw()
        {
            _rotIndex -= 1;
            if (_rotIndex < 0)
                _rotIndex = 3;
        }

        public void RotCw()
        {
            _rotIndex += 1;
            if (_rotIndex > 3)
                _rotIndex = 0;
        }
    }

    public class BlockI : Block
    {
        public BlockI()
        {
            Pixels = new[]
            {
                // Point Up, Flat Down
                new Pixel(new []{ 48, 199, 239, 255, 0, 1}),
                new Pixel(new []{ 48, 199, 239, 255, 1, 1}),
                new Pixel(new []{ 48, 199, 239, 255, 2, 1}),
                new Pixel(new []{ 48, 199, 239, 255, 3, 1}),

                // Point Right, Flat Left
                new Pixel(new []{ 48, 199, 239, 255, 2, 0}),
                new Pixel(new []{ 48, 199, 239, 255, 2, 1}),
                new Pixel(new []{ 48, 199, 239, 255, 2, 2}),
                new Pixel(new []{ 48, 199, 239, 255, 2, 3}),

                // Point Down, Flat Up
                new Pixel(new []{ 48, 199, 239, 255, 0, 2}),
                new Pixel(new []{ 48, 199, 239, 255, 1, 2}),
                new Pixel(new []{ 48, 199, 239, 255, 2, 2}),
                new Pixel(new []{ 48, 199, 239, 255, 3, 2}),

                // Point Left, Float Right
                new Pixel(new []{ 48, 199, 239, 255, 1, 0}),
                new Pixel(new []{ 48, 199, 239, 255, 1, 1}),
                new Pixel(new []{ 48, 199, 239, 255, 1, 2}),
                new Pixel(new []{ 48, 199, 239, 255, 2, 3})
            };
        }
    }

    public class BlockO : Block
    {
        public BlockO()
        {
            Pixels = new[]
            {
                // Point Up, Flat Down
                new Pixel(new []{ 247, 212, 7, 255, 1, 1}),
                new Pixel(new []{ 247, 212, 7, 255, 2, 1}),
                new Pixel(new []{ 247, 212, 7, 255, 1, 2}),
                new Pixel(new []{ 247, 212, 7, 255, 2, 2}),

                // Point Right, Flat Left
                new Pixel(new []{ 247, 212, 7, 255, 1, 1}),
                new Pixel(new []{ 247, 212, 7, 255, 2, 1}),
                new Pixel(new []{ 247, 212, 7, 255, 1, 2}),
                new Pixel(new []{ 247, 212, 7, 255, 2, 2}),

                // Point Down, Flat Up
                new Pixel(new []{ 247, 212, 7, 255, 1, 1}),
                new Pixel(new []{ 247, 212, 7, 255, 2, 1}),
                new Pixel(new []{ 247, 212, 7, 255, 1, 2}),
                new Pixel(new []{ 247, 212, 7, 255, 2, 2}),

                // Point Left, Float Right
                new Pixel(new []{ 247, 212, 7, 255, 1, 1}),
                new Pixel(new []{ 247, 212, 7, 255, 2, 1}),
                new Pixel(new []{ 247, 212, 7, 255, 1, 2}),
                new Pixel(new []{ 247, 212, 7, 255, 2, 2}),
            };
        }
    }

    public class BlockT : Block
    {
        public BlockT()
        {
            Pixels = new[]
            {
                // Point Up, Flat Down
                new Pixel(new []{ 173, 78, 158, 255, 0, 1}),
                new Pixel(new []{ 173, 78, 158, 255, 1, 1}),
                new Pixel(new []{ 173, 78, 158, 255, 2, 1}),
                new Pixel(new []{ 173, 78, 158, 255, 1, 1}),

                // Point Right, Flat Left
                new Pixel(new []{ 173, 78, 158, 255, 1, 0}),
                new Pixel(new []{ 173, 78, 158, 255, 1, 1}),
                new Pixel(new []{ 173, 78, 158, 255, 1, 2}),
                new Pixel(new []{ 173, 78, 158, 255, 2, 1}),

                // Point Down, Flat Up
                new Pixel(new []{ 173, 78, 158, 255, 0, 1}),
                new Pixel(new []{ 173, 78, 158, 255, 1, 1}),
                new Pixel(new []{ 173, 78, 158, 255, 2, 1}),
                new Pixel(new []{ 173, 78, 158, 255, 1, 2}),

                // Point Left, Float Right
                new Pixel(new []{ 173, 78, 158, 255, 0, 1}),
                new Pixel(new []{ 173, 78, 158, 255, 1, 0}),
                new Pixel(new []{ 173, 78, 158, 255, 1, 1}),
                new Pixel(new []{ 173, 78, 158, 255, 1, 2}),
            };
        }
    }

    public class BlockS : Block
    {
        public BlockS()
        {
            Pixels = new[]
            {
                // Point Up, Flat Down
                new Pixel(new []{ 0, 255, 1, 255, 0, 1}),
                new Pixel(new []{ 0, 255, 1, 255, 1, 1}),
                new Pixel(new []{ 0, 255, 1, 255, 1, 0}),
                new Pixel(new []{ 0, 255, 1, 255, 2, 0}),

                // Point Right, Flat Left
                new Pixel(new []{ 0, 255, 1, 255, 1, 0}),
                new Pixel(new []{ 0, 255, 1, 255, 1, 1}),
                new Pixel(new []{ 0, 255, 1, 255, 2, 1}),
                new Pixel(new []{ 0, 255, 1, 255, 2, 2}),

                // Point Down, Flat Up
                new Pixel(new []{ 0, 255, 1, 255, 0, 2}),
                new Pixel(new []{ 0, 255, 1, 255, 1, 2}),
                new Pixel(new []{ 0, 255, 1, 255, 1, 1}),
                new Pixel(new []{ 0, 255, 1, 255, 2, 1}),

                // Point Left, Float Right
                new Pixel(new []{ 0, 255, 1, 255, 0, 0}),
                new Pixel(new []{ 0, 255, 1, 255, 0, 1}),
                new Pixel(new []{ 0, 255, 1, 255, 1, 1}),
                new Pixel(new []{ 0, 255, 1, 255, 1, 2}),
            };
        }
    }

    public class BlockZ : Block
    {
        public BlockZ()
        {
            Pixels = new[]
            {
                // Point Up, Flat Down
                new Pixel(new []{ 255, 1, 0, 255, 0, 0}),
                new Pixel(new []{ 255, 1, 0, 255, 1, 0}),
                new Pixel(new []{ 255, 1, 0, 255, 1, 1}),
                new Pixel(new []{ 255, 1, 0, 255, 2, 2}),

                // Point Right, Flat Left
                new Pixel(new []{ 255, 1, 0, 255, 2, 0}),
                new Pixel(new []{ 255, 1, 0, 255, 1, 1}),
                new Pixel(new []{ 255, 1, 0, 255, 2, 1}),
                new Pixel(new []{ 255, 1, 0, 255, 1, 2}),

                // Point Down, Flat Up
                new Pixel(new []{ 255, 1, 0, 255, 0, 1}),
                new Pixel(new []{ 255, 1, 0, 255, 1, 1}),
                new Pixel(new []{ 255, 1, 0, 255, 1, 2}),
                new Pixel(new []{ 255, 1, 0, 255, 2, 2}),

                // Point Left, Float Right
                new Pixel(new []{ 255, 1, 0, 255, 0, 1}),
                new Pixel(new []{ 255, 1, 0, 255, 0, 2}),
                new Pixel(new []{ 255, 1, 0, 255, 1, 0}),
                new Pixel(new []{ 255, 1, 0, 255, 1, 1}),
            };
        }
    }

    public class BlockJ : Block
    {
        public BlockJ()
        {
            Pixels = new[]
            {
                // Point Up, Flat Down
                new Pixel(new []{ 0, 0, 255, 255, 0, 0}),
                new Pixel(new []{ 0, 0, 255, 255, 0, 0}),
                new Pixel(new []{ 0, 0, 255, 255, 0, 1}),
                new Pixel(new []{ 0, 0, 255, 255, 0, 2}),

                // Point Right, Flat Left
                new Pixel(new []{ 0 ,0, 255, 255, 1, 0}),
                new Pixel(new []{ 0, 0, 255, 255, 2, 0}),
                new Pixel(new []{ 0, 0, 255, 255, 1, 1}),
                new Pixel(new []{ 0, 0, 255, 255, 1, 2}),

                // Point Down, Flat Up
                new Pixel(new []{ 0, 0, 255, 255, 0, 1}),
                new Pixel(new []{ 0, 0, 255, 255, 1, 1}),
                new Pixel(new []{ 0, 0, 255, 255, 2, 1}),
                new Pixel(new []{ 0, 0, 255, 255, 2, 2}),

                // Point Left, Float Right
                new Pixel(new []{ 0, 0, 255, 255, 0, 2}),
                new Pixel(new []{ 0, 0, 255, 255, 1, 2}),
                new Pixel(new []{ 0, 0, 255, 255, 1, 1}),
                new Pixel(new []{ 0, 0, 255, 255, 1, 0}),
            };
        }
    }

    public class BlockL : Block
    {
        public BlockL()
        {
            Pixels = new[]
            {
                // Point Up, Flat Down
                new Pixel(new []{ 239, 121, 34, 255, 0, 1}),
                new Pixel(new []{ 239, 121, 34, 255, 1, 1}),
                new Pixel(new []{ 239, 121, 34, 255, 1, 2}),
                new Pixel(new []{ 239, 121, 34, 255, 2, 0}),

                // Point Right, Flat Left
                new Pixel(new []{ 239, 121, 34, 255, 1, 0}),
                new Pixel(new []{ 239, 121, 34, 255, 1, 1}),
                new Pixel(new []{ 239, 121, 34, 255, 1, 2}),
                new Pixel(new []{ 239, 121, 34, 255, 2, 2}),

                // Point Down, Flat Up
                new Pixel(new []{ 239, 121, 34, 255, 0, 1}),
                new Pixel(new []{ 239, 121, 34, 255, 0, 2}),
                new Pixel(new []{ 239, 121, 34, 255, 1, 1}),
                new Pixel(new []{ 239, 121, 34, 255, 1, 2}),

                // Point Left, Float Right
                new Pixel(new []{ 239, 121, 34, 255, 0, 0}),
                new Pixel(new []{ 239, 121, 34, 255, 1, 0}),
                new Pixel(new []{ 239, 121, 34, 255, 1, 1}),
                new Pixel(new []{ 239, 121, 34, 255, 1, 2}),
            };
        }
    }
}
