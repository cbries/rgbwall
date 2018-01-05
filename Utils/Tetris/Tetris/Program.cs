namespace Tetris
{
    class Program
    {
        static void Main(string[] args)
        {
            DrawConsole drawEngine = new DrawConsole();

            Game game = new Game(drawEngine);
            game.Init();
            game.Play();
            game.Clean();
        }
    }
}
