using Client.Enums;
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
        private static readonly Typeface cardInfoTypeface = new Typeface(new FontFamily("Calibri"), FontStyles.Normal, FontWeights.Bold, FontStretches.Normal);
        private const double cardInfoFontSize = 60.0;
        private const double cardNamePositionY = 584.0;
        private const double cardTypePositionY = 25;

        private static readonly Typeface spellInfoTypeface = new Typeface(new FontFamily("Calibri"), FontStyles.Normal, FontWeights.UltraBold, FontStretches.Normal);
        private const double spellInfoFontSize = 45.0;
        private const double spellInfoTextWidth = 660;
        private static readonly Point spellInfoPosition = new Point(50, 675);

        private static readonly Typeface statsTypeface = new Typeface(new FontFamily("Calibri"), FontStyles.Italic, FontWeights.Bold, FontStretches.Normal);
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

        public UInt32 Id { get; }

        public string Name { get; set; }

        public CreatureTypes Type { get; }

        public byte Hp 
        {
            get => health;
            set 
            {
                health = value;
                ReloadStats();
            }
        }
        public byte Damage { get; }

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

        public byte Defense { get; }

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

        public Spell Spell { get; }

        public Card(UInt32 id, CreatureTypes type, byte hp, byte damage, byte mana, byte defense, Spell spell)
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

                // Card stats
                drawingContext.DrawText(new FormattedText(DamageModified.ToString(), cultureInfo, FlowDirection.LeftToRight, statsTypeface, statsFontSize, GetStatBrush(CardStats.Damage)), damagePosition);
                drawingContext.DrawText(new FormattedText(mana.ToString(), cultureInfo, FlowDirection.LeftToRight, statsTypeface, statsFontSize, Brushes.White), manaPosition);
                drawingContext.DrawText(new FormattedText(health.ToString(), cultureInfo, FlowDirection.LeftToRight, statsTypeface, statsFontSize, Brushes.White), hpPosition);
                drawingContext.DrawText(new FormattedText(DefenseModified.ToString(), cultureInfo, FlowDirection.LeftToRight, statsTypeface, statsFontSize, GetStatBrush(CardStats.Defense)), defensePosition);
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
                // Card
                drawingContext.DrawImage(creature, new Rect((cardTemplate.PixelWidth - creatureImageWidth) / 2, creatureImageHeightOffset, creatureImageWidth, creatureImageHeight));
                drawingContext.DrawImage(cardTemplate, new Rect(0, 0, cardTemplate.PixelWidth, cardTemplate.PixelHeight));

                // Type
                var type = new FormattedText(Type.ToString(), cultureInfo, FlowDirection.LeftToRight, cardInfoTypeface, cardInfoFontSize, Brushes.Black);
                drawingContext.DrawText(type, new Point(cardTemplate.PixelWidth / 2 - type.Width / 2, cardTypePositionY));

                // Name
                var name = new FormattedText(Name, cultureInfo, FlowDirection.LeftToRight, cardInfoTypeface, cardInfoFontSize, Brushes.Black);
                drawingContext.DrawText(name, new Point(cardTemplate.PixelWidth / 2 - name.Width / 2, cardNamePositionY));

                // Spell
                if (Spell != null)
                {
                    var spell = new FormattedText(Spell.Info, cultureInfo, FlowDirection.LeftToRight, spellInfoTypeface, spellInfoFontSize, Brushes.Black)
                    {
                        MaxTextWidth = spellInfoTextWidth
                    };
                    drawingContext.DrawText(spell, spellInfoPosition);
                }
            }

            var bmp = new RenderTargetBitmap(cardTemplate.PixelWidth, cardTemplate.PixelHeight, 96, 96, PixelFormats.Pbgra32);
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

        private Brush GetStatBrush(CardStats cardStat)
        {
            var modifier = 0;
            var stat = 0;

            switch (cardStat)
            {
                case CardStats.Defense:
                    modifier = defenseModifier;
                    stat = Defense;
                    break;
                case CardStats.Damage:
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
