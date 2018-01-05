namespace Tetris
{
    public class Block
    {
        public Pixel[] Pixels { get; protected set; }
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
