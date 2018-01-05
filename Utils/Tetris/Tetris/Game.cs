using System.Diagnostics;

namespace Tetris
{
    public class Game : IGame
    {
        private readonly IDraw _drawEngine = null;
        private readonly Stopwatch _stopwatch = new Stopwatch();

        private int _updateRender = 50; // msecs
        private long _lastRenderUpdate = 0;

        private int _updateMove = 1000; // msecs
        private long _lastMoveUpdate = 0;

        #region IGame

        private const int _width = 10;
        private const int _height = 20;

        private Pixel[] _pixels = new Pixel[_width * _height];

        public int Width { get { return _width; } }
        public int Height { get { return _height; } }
        public Pixel[] Pixels { get { return _pixels; } }

        #endregion

        public Game(IDraw drawEngine)
        {
            _drawEngine = drawEngine;
            _stopwatch.Start();
        }

        private bool ShouldRenderUpdate()
        {
            var current = _stopwatch.ElapsedMilliseconds;
            var delta = current - _lastRenderUpdate;
            if (delta > _updateRender)
                return true;
            return false;
        }

        private bool ShouldMoveUpdate()
        {
            var current = _stopwatch.ElapsedMilliseconds;
            var delta = current - _lastMoveUpdate;
            if (delta > _updateMove)
                return true;
            return false;
        }

        public void Init()
        {
            _lastRenderUpdate = _stopwatch.ElapsedMilliseconds;
            _lastMoveUpdate = _stopwatch.ElapsedMilliseconds;
        }

        public void Play()
        {
            while (true)
            {
                ProcessInput();

                Update();

                if (ShouldRenderUpdate())
                {
                    Render();

                    _lastRenderUpdate = _stopwatch.ElapsedMilliseconds;
                }
            }
        }

        public void Clean()
        {
            // tbd
        }

        private void ProcessInput()
        {
            // tbd
        }

        private bool Update()
        {
            if (!ShouldMoveUpdate())
                return false;

            // do update

            _lastMoveUpdate = _stopwatch.ElapsedMilliseconds;

            return true;
        }

        private void Render()
        {
            if (_drawEngine == null)
                return;

            _drawEngine.Render();
        }
    }
}
