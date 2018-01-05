using System;

namespace Tetris
{
    public class Pixel
    {
        public Pixel(int[] values)
        {
            if (values.Length != 6)
                throw new ArgumentException("incorrect length");

            X = values[0];
            Y = values[1];
            Red = values[2];
            Green = values[3];
            Blue = values[4];
            Brightness = values[5];
        }

        public int Red { get; set; }
        public int Green { get; set; }
        public int Blue { get; set; }
        public int Brightness { get; set; }

        public int X { get; set; }
        public int Y { get; set; }

        public bool HasValue()
        {
            if (Red == 0 && Green == 0 && Blue == 0)
                return false;
            return true;
        }
    }
}
