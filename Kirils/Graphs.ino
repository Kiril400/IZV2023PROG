
void renderGraph(byte position, byte level, byte graphWidth, byte graphHeight, int lowerBound, int upperBound, int *dataPoints, String graphLabel) {
  int highestValue = -32000;
  int lowestValue = 32000;

  // Loop through the data array to find the max and min values
  for (byte index = 0; index < 15; index++) {
    if (dataPoints[index] > highestValue) highestValue = dataPoints[index];
    if (dataPoints[index] < lowestValue) lowestValue = dataPoints[index];
  }
  lcd.setCursor(16, 0); lcd.print(highestValue);  // Display the highest value on the graph
  lcd.setCursor(16, 1); lcd.print(graphLabel);   // Display the graph label
  lcd.setCursor(16, 2); lcd.print(dataPoints[14]);  // Display the last value in the data array
  lcd.setCursor(16, 3); lcd.print(lowestValue);  // Display the lowest value on the graph

  // Rendering each column of the graph
  for (byte index = 0; index < graphWidth; index++) {                  
    int currentValue = dataPoints[index];
    currentValue = constrain(currentValue, lowerBound, upperBound);
    byte fullBlocks, fractionalBlocks;

    // Calculate the number of full blocks needed for the graph bar
    if (dataPoints[index] > lowerBound)
      fullBlocks = floor((float)(dataPoints[index] - lowerBound) / (upperBound - lowerBound) * graphHeight * 10);
    else fullBlocks = 0;
    fractionalBlocks = (float)(fullBlocks % 10) * 8 / 10;  // Calculate remaining fraction of the block
    fullBlocks = fullBlocks / 10;

    // Rendering each row of the graph
    for (byte row = 0; row < graphHeight; row++) {     
      if (row < fullBlocks && fullBlocks > 0) {       
        lcd.setCursor(index, (level - row));        
        lcd.write(0);  // Write full blocks
      }
      if (row >= fullBlocks) {                    
        lcd.setCursor(index, (level - row));
        if (fractionalBlocks > 0) lcd.write(fractionalBlocks);  // Write fractional blocks
        else lcd.write(16);  // Empty block if no fractional part
        for (byte k = row + 1; k < graphHeight; k++) {  // Fill the rest with empty blocks
          lcd.setCursor(index, (level - k));
          lcd.write(16);
        }
        break;
      }
    }
  }
}
  // Hopefully I have not messed up some coordinates :/
// Needs check-up when components arrive 
