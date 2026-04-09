

import { Size } from '../../../core';

export class TextLayout {
    // Layout common
    // alignment
    public horizontalAlign = 0;// Enum  // both
    public verticalAlign = 0;// Enum // both

    public wrapping = true; // both
    public overFlow = 0;// Enum  // both

    public lineHeight = 10; // both

    // bmfont extra part
    public maxLineWidth = 0; // bmfont
    public spacingX = 0; // bmfont

    // bmfont used temp value
    public textWidthTemp = 0;
    public textHeightTemp = 0;
    public textDimensions = new Size();

    public horizontalKerning: number[] = [];
    public numberOfLines = 1;

    public linesOffsetX: number[] = []; // layout
    public letterOffsetY = 0;

    public tailoredTopY = 0;
    public tailoredBottomY = 0;

    public textDesiredHeight = 0;
    public linesWidth: number[] = [];

    public reset(): void {
        this.horizontalAlign = 0;
        this.verticalAlign = 0;
        this.wrapping = true;
        this.overFlow = 0;
        this.lineHeight = 10;
        this.maxLineWidth = 0;
        this.spacingX = 0;
        this.textWidthTemp = 0;
        this.textHeightTemp = 0;
        this.textDimensions.set(0, 0);
        this.horizontalKerning.length = 0;
        this.numberOfLines = 1;
        this.linesOffsetX.length = 0;
        this.letterOffsetY = 0;
        this.tailoredTopY = 0;
        this.tailoredBottomY = 0;
        this.textDesiredHeight = 0;
        this.linesWidth.length = 0;
    }
}
