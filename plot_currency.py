"""
Currency Conversion Graph Plotter
This script creates a bar chart to visualize currency conversion results.
"""

import csv
import os
import matplotlib.pyplot as plt


def load_currency_conversion_data(path="currency_conversion_data.csv"):
    """
    Load currency conversion data from a CSV file.
    
    Args:
        path: The path to the CSV file (default: "currency_conversion_data.csv")
    
    Returns:
        Two lists: currencies and their corresponding amounts
    """
    # Initialize empty lists to store the data
    currencies = []
    amounts = []
    
    # Check if the file exists before trying to open it
    if not os.path.exists(path):
        print(f"CSV file not found: {path}")
        return currencies, amounts
    
    # Open the CSV file and read the data
    with open(path, "r", newline="") as file:
        # Create a CSV reader that treats the first row as column headers
        reader = csv.DictReader(file)
        
        # Read each row from the CSV file
        for row in reader:
            try:
                # Extract the currency code from the row
                currency_code = row["currency"]
                
                # Convert the amount string to a float number
                amount_value = float(row["amount"])
                
                # Add the currency and amount to our lists
                currencies.append(currency_code)
                amounts.append(amount_value)
                
            except (ValueError, KeyError):
                # If there's an error reading a row, skip it and continue
                continue
    
    # Return the lists we created
    return currencies, amounts


def main():
    """
    Main function that creates and displays the currency conversion graph.
    """
    # Load the currency conversion data from the CSV file
    currencies, amounts = load_currency_conversion_data()
    
    # Check if we have enough data to create a graph
    # We need at least 2 currencies (source and target)
    if len(currencies) < 2 or len(amounts) < 2:
        print("No data to plot.")
        return
    
    # Create a new figure (graph window) with specific size
    # Width: 10 inches, Height: 6 inches
    plt.figure(figsize=(10, 6))
    
    # Define colors for the bars
    # First bar will be green, second bar will be blue
    bar_color_1 = '#2ecc71'  # Green color
    bar_color_2 = '#3498db'  # Blue color
    bar_colors = [bar_color_1, bar_color_2]
    
    # Create the bar chart
    # currencies: labels for each bar (x-axis)
    # amounts: values for each bar (y-axis)
    # color: colors for the bars
    # alpha: transparency (0.7 = 70% opaque)
    # edgecolor: color of the bar borders
    # linewidth: thickness of the bar borders
    bars = plt.bar(
        currencies, 
        amounts, 
        color=bar_colors, 
        alpha=0.7, 
        edgecolor='black', 
        linewidth=1.5
    )
    
    # Add value labels on top of each bar
    # Loop through each currency and its corresponding amount
    for index in range(len(currencies)):
        currency = currencies[index]
        amount = amounts[index]
        
        # Format the amount with commas and 2 decimal places
        formatted_amount = f'{amount:,.2f}'
        
        # Add text label on top of the bar
        plt.text(
            index,           # x position (bar index)
            amount,          # y position (bar height)
            formatted_amount, # text to display
            ha='center',     # horizontal alignment (center)
            va='bottom',     # vertical alignment (bottom of text)
            fontweight='bold', # make text bold
            fontsize=11       # font size
        )
    
    # Set the label for the x-axis (horizontal axis)
    plt.xlabel("Currency", fontsize=12, fontweight='bold')
    
    # Set the label for the y-axis (vertical axis)
    plt.ylabel("Amount", fontsize=12, fontweight='bold')
    
    # Set the title of the graph
    plt.title("Currency Conversion Comparison", fontsize=14, fontweight='bold', pad=20)
    
    # Add a grid to make it easier to read values
    # linestyle: dashed lines
    # alpha: transparency (0.3 = 30% opaque)
    # axis: only show grid lines for y-axis
    plt.grid(True, linestyle="--", alpha=0.3, axis='y')
    
    # Add conversion rate text if we have exactly 2 currencies
    if len(amounts) == 2:
        # Calculate the exchange rate
        source_amount = amounts[0]
        target_amount = amounts[1]
        
        # Avoid division by zero
        if source_amount > 0:
            exchange_rate = target_amount / source_amount
        else:
            exchange_rate = 0
        
        # Create the exchange rate text
        source_currency = currencies[0]
        target_currency = currencies[1]
        rate_text = f'Exchange Rate: 1 {source_currency} = {exchange_rate:.4f} {target_currency}'
        
        # Calculate position for the text (90% of the maximum bar height)
        max_amount = max(amounts)
        text_y_position = max_amount * 0.9
        
        # Create a box style for the text background
        text_box_style = dict(
            boxstyle='round',      # rounded corners
            facecolor='wheat',      # background color
            alpha=0.5               # transparency
        )
        
        # Add the text to the graph
        plt.text(
            0.5,                    # x position (center of graph)
            text_y_position,        # y position
            rate_text,              # text to display
            ha='center',            # horizontal alignment
            transform=plt.gca().transData,  # use data coordinates
            fontsize=10,            # font size
            bbox=text_box_style     # text box style
        )
    
    # Adjust the layout to prevent labels from being cut off
    plt.tight_layout()
    
    # Display the graph
    plt.show()


# This code runs when the script is executed directly (not imported)
if __name__ == "__main__":
    main()
