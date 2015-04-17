using CardGameWPF.Data;
using CardGameWPF.Enums;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace CardGameWPF.Game
{
    public abstract class Card
    {
        private BitmapSource cardTemplateImage;
        private BitmapSource image;

        private static int creatureImageWidth = 656;
        private static int creatureImageHeight = 480;
        private static int creatureImageHeightOffset = 104;

        private static CultureInfo cultureInfo = new CultureInfo("en-GB");
        private static Typeface cardInfoTypeface = new Typeface(new FontFamily("Calibri"), FontStyles.Normal, FontWeights.Bold, FontStretches.Normal);
        private static double cardInfofontSize = 60.0;
        private static double cardNamePositionY = 584.0;
        private static double cardTypePositionY = 25;

        private static Typeface statsTypeface = new Typeface(new FontFamily("Calibri"), FontStyles.Italic, FontWeights.Bold, FontStretches.Normal);
        private static double statsFontSize = 100.0;
        private static Point damagePosition = new Point(35.0, 0.0);
        private static Point manaPosition = new Point(640.0, 0.0);
        private static Point hpPosition = new Point(35.0, 920.0);
        private static Point defensePosition = new Point(640.0, 920.0);

        public bool Selected { get; set; }
        public UInt32 Id { get; private set; }
        public string Name { get; set; }
        public CreatureTypes Type { get; private set; }
        public byte Hp { get; private set; }
        public byte Damage { get; private set; }
        public byte Mana { get; private set; }
        public byte Defense { get; private set; }
        public string ImageUri { get; set; }
        public BitmapSource Image
        {
            get
            {
                if (image == null)
                    ReloadStats();

                return Selected ? SelectedCard() : image;
            }
        }

        public Card(UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense)
        {
            Id = id;
            Type = type;
            Hp = hp;
            Damage = damage;
            Mana = mana;
            Defense = defense;
        }

        // Unloads images from memory
        public void UnloadImages()
        {
            cardTemplateImage = null;
            image = null;
        }

        // Creates card template
        private void CreateCardTemplateImage()
        {
            BitmapFrame cardTemplate = BitmapFrame.Create(new Uri("Assets/CardTemplate.png", UriKind.Relative));
            BitmapFrame creature = BitmapFrame.Create(new Uri(ImageUri, UriKind.Relative));

            // Draws the images into a DrawingVisual component
            DrawingVisual drawingVisual = new DrawingVisual();
            using (DrawingContext drawingContext = drawingVisual.RenderOpen())
            {
                // Card
                drawingContext.DrawImage(creature, new Rect((cardTemplate.PixelWidth - creatureImageWidth) / 2, creatureImageHeightOffset, creatureImageWidth, creatureImageHeight));
                drawingContext.DrawImage(cardTemplate, new Rect(0, 0, cardTemplate.PixelWidth, cardTemplate.PixelHeight));

                // Type
                var type = new FormattedText(Type.ToString(), cultureInfo, FlowDirection.LeftToRight, cardInfoTypeface, cardInfofontSize, Brushes.Black);
                drawingContext.DrawText(type, new Point(cardTemplate.PixelWidth / 2 - type.Width / 2, cardTypePositionY));

                // Name
                var name = new FormattedText(Name, cultureInfo, FlowDirection.LeftToRight, cardInfoTypeface, cardInfofontSize, Brushes.Black);
                drawingContext.DrawText(name, new Point(cardTemplate.PixelWidth / 2 - name.Width / 2, cardNamePositionY));
            }

            RenderTargetBitmap bmp = new RenderTargetBitmap(cardTemplate.PixelWidth, cardTemplate.PixelHeight, 96, 96, PixelFormats.Pbgra32);
            bmp.Render(drawingVisual);

            cardTemplateImage = bmp;
        }

        // Reloads stats on card
        private void ReloadStats()
        {
            if (cardTemplateImage == null)
                CreateCardTemplateImage();

            // Draws the images into a DrawingVisual component
            DrawingVisual drawingVisual = new DrawingVisual();
            using (DrawingContext drawingContext = drawingVisual.RenderOpen())
            {
                // Card
                drawingContext.DrawImage(cardTemplateImage, new Rect(0, 0, cardTemplateImage.PixelWidth, cardTemplateImage.PixelHeight));

                // Card stats
                drawingContext.DrawText(new FormattedText(Damage.ToString(), cultureInfo, FlowDirection.LeftToRight, statsTypeface, statsFontSize, Brushes.White), damagePosition);
                drawingContext.DrawText(new FormattedText(Mana.ToString(), cultureInfo, FlowDirection.LeftToRight, statsTypeface, statsFontSize, Brushes.White), manaPosition);
                drawingContext.DrawText(new FormattedText(Hp.ToString(), cultureInfo, FlowDirection.LeftToRight, statsTypeface, statsFontSize, Brushes.White), hpPosition);
                drawingContext.DrawText(new FormattedText(Defense.ToString(), cultureInfo, FlowDirection.LeftToRight, statsTypeface, statsFontSize, Brushes.White), defensePosition);
            }

            // Converts the Visual (DrawingVisual) into a BitmapSource
            RenderTargetBitmap bmp = new RenderTargetBitmap(cardTemplateImage.PixelWidth, cardTemplateImage.PixelHeight, 96, 96, PixelFormats.Pbgra32);
            bmp.Render(drawingVisual);

            image = bmp;
        }

        // Visually selects card
        private BitmapSource SelectedCard()
        {
            DrawingVisual drawingVisual = new DrawingVisual();
            using (DrawingContext drawingContext = drawingVisual.RenderOpen())
            {
                // Card
                drawingContext.DrawImage(image, new Rect(0, 0, image.PixelWidth, image.PixelHeight));

                // Draw border
                Pen pen = new Pen(Brushes.Yellow, 50.0);
                drawingContext.DrawLine(pen, new Point(0, 0), new Point(image.Width, 0));
                drawingContext.DrawLine(pen, new Point(image.Width, 0), new Point(image.Width, image.Height));
                drawingContext.DrawLine(pen, new Point(image.Width, image.Height), new Point(0, image.Height));
                drawingContext.DrawLine(pen, new Point(0, image.Height), new Point(0, 0));
            }

            RenderTargetBitmap bmp = new RenderTargetBitmap(cardTemplateImage.PixelWidth, cardTemplateImage.PixelHeight, 96, 96, PixelFormats.Pbgra32);
            bmp.Render(drawingVisual);

            return bmp;
        }
    }
}
