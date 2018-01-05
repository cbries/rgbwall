namespace Tetris
{
    public interface IGame
    {
        int Width { get; }
        int Height { get; }

        Pixel[] Pixels { get; }
    }

    public static class GameUtils
    {
        public static Pixel GetPixel(this IGame game, int x, int y)
        {
            var idx = x + y * game.Width;
            if (idx < 0)
                idx = 0;
            else if (idx > game.Width * game.Height)
                idx = game.Width * game.Height - 1;
            return game.Pixels[idx];
        }
    }
}
