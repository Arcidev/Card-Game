using Client.Data;
using Client.Enums;
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace Client.Game
{
    public abstract class Card
    {
        private static readonly Dictionary<SelectionType, Brush> selectionColors = new Dictionary<SelectionType,Brush>()
        {
            { SelectionType.None,                   null            },
            { SelectionType.Selected,               Brushes.Yellow  },
            { SelectionType.BasicDamageAttackable,  Brushes.Red     },
            { SelectionType.SpellDamaeAttackable,   Brushes.Blue    }
        };

        private BitmapSource cardTemplateImage;
        private BitmapSource image;

        private static readonly int creatureImageWidth = 656;
        private static readonly int creatureImageHeight = 480;
        private static readonly int creatureImageHeightOffset = 104;

        private static readonly CultureInfo cultureInfo = new CultureInfo("en-GB");
        private static readonly Typeface cardInfoTypeface = new Typeface(new FontFamily("Calibri"), FontStyles.Normal, FontWeights.Bold, FontStretches.Normal);
        private static readonly double cardInfofontSize = 60.0;
        private static readonly double cardNamePositionY = 584.0;
        private static readonly double cardTypePositionY = 25;

        private static readonly Typeface statsTypeface = new Typeface(new FontFamily("Calibri"), FontStyles.Italic, FontWeights.Bold, FontStretches.Normal);
        private static readonly double statsFontSize = 100.0;
        private static readonly Point damagePosition = new Point(35.0, 0.0);
        private static readonly Point manaPosition = new Point(640.0, 0.0);
        private static readonly Point hpPosition = new Point(35.0, 920.0);
        private static readonly Point defensePosition = new Point(640.0, 920.0);

        public SelectionType SelectionType { get; set; }
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

                return (SelectionType != SelectionType.None) ? SelectedCard() : image;
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
            Brush borderColor = selectionColors[SelectionType];
            if (borderColor == null)
                return image;

            DrawingVisual drawingVisual = new DrawingVisual();
            using (DrawingContext drawingContext = drawingVisual.RenderOpen())
            {
                // Card
                drawingContext.DrawImage(image, new Rect(0, 0, image.PixelWidth, image.PixelHeight));

                // Draw border
                Pen pen = new Pen(borderColor, 50.0);
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
