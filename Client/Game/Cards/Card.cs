using Client.Logic.Enums;
using System;
using System.Collections.Generic;
using System.Globalization;
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
            { SelectionType.SpellUsable,            Brushes.Blue    }
        };
        
        private BitmapSource cardTemplateImage;
        private BitmapSource image;

        private const int creatureImageWidth = 656;
        private const int creatureImageHeight = 480;
        private const int creatureImageHeightOffset = 104;

        private static readonly CultureInfo cultureInfo = new CultureInfo("en-GB");
        private static readonly Typeface normalFont = new Typeface(new FontFamily("Arial"), FontStyles.Normal, FontWeights.Bold, FontStretches.Normal);
        private const double cardInfoFontSize = 60.0;
        private const double cardNamePositionY = 584.0;
        private const double cardTypePositionY = 25;

        private const double spellInfoFontSize = 50;
        private const double spellInfoTextWidth = 640;
        private static readonly Point spellInfoPosition = new Point(50, 675);

        private static readonly Typeface italicFont = new Typeface(new FontFamily("Arial"), FontStyles.Italic, FontWeights.Bold, FontStretches.Normal);
        private const double statsFontSize = 100.0;
        private static readonly Point damagePosition = new Point(35.0, 0.0);
        private static readonly Point manaPosition = new Point(640.0, 0.0);
        private static readonly Point hpPosition = new Point(35.0, 920.0);
        private static readonly Point defensePosition = new Point(640.0, 920.0);

        private byte health;
        private byte mana;
        private sbyte defenseModifier;
        private sbyte damageModifier;

        public SelectionType SelectionType { get; set; }

        public UInt32 Id { get; set; }

        public string Name { get; set; }

        public CreatureType Type { get; set; }

        public byte Hp 
        {
            get => health;
            set 
            {
                health = value;
                ReloadStats();
            }
        }

        public byte Damage { get; set; }

        public sbyte DamageModifier
        {
            get => damageModifier;
            set
            {
                damageModifier = value;
                ReloadStats();
            }
        }

        public byte DamageModified
        {
            get
            {
                if (damageModifier < 0)
                    if (Damage < -damageModifier)
                        return 0;

                return (byte)(Damage + damageModifier);
            }
        }

        public byte Mana
        {
            get => mana;
            set
            {
                mana = value;
                ReloadStats();
            }
        }

        public byte Defense { get; set; }

        public sbyte DefenseModifier
        {
            get => defenseModifier;
            set
            {
                defenseModifier = value;
                ReloadStats();
            }
        }

        public byte DefenseModified
        {
            get
            {
                if (defenseModifier < 0)
                    if (Defense < -defenseModifier)
                        return 0;

                return (byte)(Defense + defenseModifier);
            }
        }

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

        public Spell Spell { get; set; }

        public Card(UInt32 id, CreatureType type, byte hp, byte damage, byte mana, byte defense, Spell spell)
        {
            Id = id;
            Type = type;
            health = hp;
            this.mana = mana;
            Damage = damage;
            Defense = defense;
            SelectionType = SelectionType.None;
            defenseModifier = 0;
            damageModifier = 0;
            Spell = spell;
        }

        // Unloads images from memory
        public void UnloadImages()
        {
            cardTemplateImage = null;
            image = null;
        }

        // Reloads stats on card
        public void ReloadStats()
        {
            if (cardTemplateImage == null)
                CreateCardTemplateImage();

            // Draws the images into a DrawingVisual component
            var drawingVisual = new DrawingVisual();
            using (var drawingContext = drawingVisual.RenderOpen())
            {
                // Card
                drawingContext.DrawImage(cardTemplateImage, new Rect(0, 0, cardTemplateImage.PixelWidth, cardTemplateImage.PixelHeight));

                var dpi = VisualTreeHelper.GetDpi(drawingVisual);
                
                // Card stats
                drawingContext.DrawText(new FormattedText(DamageModified.ToString(), cultureInfo, FlowDirection.LeftToRight, italicFont, statsFontSize, GetStatBrush(CardStat.Damage), dpi.PixelsPerDip), damagePosition);
                drawingContext.DrawText(new FormattedText(mana.ToString(), cultureInfo, FlowDirection.LeftToRight, italicFont, statsFontSize, Brushes.White, dpi.PixelsPerDip), manaPosition);
                drawingContext.DrawText(new FormattedText(health.ToString(), cultureInfo, FlowDirection.LeftToRight, italicFont, statsFontSize, Brushes.White, dpi.PixelsPerDip), hpPosition);
                drawingContext.DrawText(new FormattedText(DefenseModified.ToString(), cultureInfo, FlowDirection.LeftToRight, italicFont, statsFontSize, GetStatBrush(CardStat.Defense), dpi.PixelsPerDip), defensePosition);
            }

            // Converts the Visual (DrawingVisual) into a BitmapSource
            var bmp = new RenderTargetBitmap(cardTemplateImage.PixelWidth, cardTemplateImage.PixelHeight, 96, 96, PixelFormats.Pbgra32);
            bmp.Render(drawingVisual);

            image = bmp;
        }

        // Creates card template
        private void CreateCardTemplateImage()
        {
            var cardTemplate = BitmapFrame.Create(new Uri("Assets/CardTemplate.png", UriKind.Relative));
            var creature = BitmapFrame.Create(new Uri(ImageUri, UriKind.Relative));

            // Draws the images into a DrawingVisual component
            var drawingVisual = new DrawingVisual();
            using (var drawingContext = drawingVisual.RenderOpen())
            {
                var dpi = VisualTreeHelper.GetDpi(drawingVisual);

                // Card
                drawingContext.DrawImage(creature, new Rect((cardTemplate.PixelWidth - creatureImageWidth) / 2, creatureImageHeightOffset, creatureImageWidth, creatureImageHeight));
                drawingContext.DrawImage(cardTemplate, new Rect(0, 0, cardTemplate.PixelWidth, cardTemplate.PixelHeight));

                // Type
                var type = new FormattedText(Type.ToString(), cultureInfo, FlowDirection.LeftToRight, normalFont, cardInfoFontSize, Brushes.Black, dpi.PixelsPerDip);
                drawingContext.DrawText(type, new Point(cardTemplate.PixelWidth / 2 - type.Width / 2, cardTypePositionY));

                // Name
                var name = new FormattedText(Name, cultureInfo, FlowDirection.LeftToRight, normalFont, cardInfoFontSize, Brushes.Black, dpi.PixelsPerDip);
                drawingContext.DrawText(name, new Point(cardTemplate.PixelWidth / 2 - name.Width / 2, cardNamePositionY));

                // Spell
                if (Spell != null)
                {
                    var spell = new FormattedText(Spell.Info, cultureInfo, FlowDirection.LeftToRight, normalFont, spellInfoFontSize, Brushes.Black, dpi.PixelsPerDip)
                    {
                        MaxTextWidth = spellInfoTextWidth
                    };
                    drawingContext.DrawText(spell, spellInfoPosition);
                }
            }

            var bmp = new RenderTargetBitmap(cardTemplate.PixelWidth, cardTemplate.PixelHeight, 96, 96, PixelFormats.Default);
            bmp.Render(drawingVisual);

            cardTemplateImage = bmp;
        }

        // Visually selects card
        private BitmapSource SelectedCard()
        {
            var borderColor = selectionColors[SelectionType];
            if (borderColor == null)
                return image;

            var drawingVisual = new DrawingVisual();
            using (var drawingContext = drawingVisual.RenderOpen())
            {
                // Card
                drawingContext.DrawImage(image, new Rect(0, 0, image.PixelWidth, image.PixelHeight));

                // Draw border
                var pen = new Pen(borderColor, 50.0);
                drawingContext.DrawLine(pen, new Point(0, 0), new Point(image.Width, 0));
                drawingContext.DrawLine(pen, new Point(image.Width, 0), new Point(image.Width, image.Height));
                drawingContext.DrawLine(pen, new Point(image.Width, image.Height), new Point(0, image.Height));
                drawingContext.DrawLine(pen, new Point(0, image.Height), new Point(0, 0));
            }

            var bmp = new RenderTargetBitmap(cardTemplateImage.PixelWidth, cardTemplateImage.PixelHeight, 96, 96, PixelFormats.Pbgra32);
            bmp.Render(drawingVisual);

            return bmp;
        }

        private Brush GetStatBrush(CardStat cardStat)
        {
            var modifier = 0;
            var stat = 0;

            switch (cardStat)
            {
                case CardStat.Defense:
                    modifier = defenseModifier;
                    stat = Defense;
                    break;
                case CardStat.Damage:
                    modifier = damageModifier;
                    stat = Damage;
                    break;
                default:
                    return Brushes.White;
            }

            if (modifier == 0 || stat == 0)
                return Brushes.White;

            return modifier > 0 ? Brushes.Green : Brushes.Red;
        }
    }
}
