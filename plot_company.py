"""
Company Stock Price Graph Plotter
This script creates a line graph to visualize company stock price history and predictions.
"""

import csv
import os
import matplotlib.pyplot as plt


def load_company_graph_data(path="company_graph_data.csv"):
    """
    Load company stock price data from a CSV file.
    
    Args:
        path: The path to the CSV file (default: "company_graph_data.csv")
    
    Returns:
        Four lists: years for actual prices, actual prices, years for predicted prices, predicted prices
    """
    # Initialize empty lists to store the data
    years_actual = []
    prices_actual = []
    years_predicted = []
    prices_predicted = []
    
    # Check if the file exists before trying to open it
    if not os.path.exists(path):
        print(f"CSV file not found: {path}")
        return years_actual, prices_actual, years_predicted, prices_predicted
    
    # Open the CSV file and read the data
    with open(path, "r", newline="") as file:
        # Create a CSV reader that treats the first row as column headers
        reader = csv.DictReader(file)
        
        # Read each row from the CSV file
        for row in reader:
            try:
                # Extract the year from the row and convert to integer
                year = int(row["year"])
                
                # Extract the price from the row and convert to float
                price = float(row["price"])
                
                # Get the kind of data (actual or predicted)
                # Default to "actual" if not specified
                data_kind = row.get("kind", "actual")
                
            except (ValueError, KeyError):
                # If there's an error reading a row, skip it and continue
                continue
            
            # Separate actual and predicted data into different lists
            if data_kind == "predicted":
                # This is a predicted price
                years_predicted.append(year)
                prices_predicted.append(price)
            else:
                # This is an actual (historical) price
                years_actual.append(year)
                prices_actual.append(price)
    
    # Return all four lists
    return years_actual, prices_actual, years_predicted, prices_predicted


def main():
    """
    Main function that creates and displays the company stock price graph.
    """
    # Load the company data from the CSV file
    years_actual, prices_actual, years_predicted, prices_predicted = load_company_graph_data()
    
    # Check if we have any data to plot
    has_actual_data = len(years_actual) > 0
    has_predicted_data = len(years_predicted) > 0
    
    if not has_actual_data and not has_predicted_data:
        print("No data to plot.")
        return
    
    # Create a new figure (graph window) with specific size
    # Width: 8 inches, Height: 4 inches
    plt.figure(figsize=(8, 4))
    
    # Plot the actual (historical) prices if we have any
    if has_actual_data:
        # Create a line plot for actual prices
        # marker="o": use circles to mark each data point
        # color="tab:blue": use blue color
        # label="Actual": label for the legend
        plt.plot(
            years_actual, 
            prices_actual, 
            marker="o", 
            color="tab:blue", 
            label="Actual"
        )
    
    # Plot the predicted prices if we have any
    if has_predicted_data:
        # Create a line plot for predicted prices
        # marker="*": use stars to mark each data point
        # color="tab:red": use red color
        # linestyle="--": use dashed line
        # label="Predicted next year": label for the legend
        plt.plot(
            years_predicted, 
            prices_predicted, 
            marker="*", 
            color="tab:red", 
            linestyle="--", 
            label="Predicted next year"
        )
    
    # Set the label for the x-axis (horizontal axis)
    plt.xlabel("Year")
    
    # Set the label for the y-axis (vertical axis)
    plt.ylabel("Price")
    
    # Set the title of the graph
    plt.title("Company Price History and Next-Year Prediction")
    
    # Add a grid to make it easier to read values
    # linestyle: dashed lines
    # alpha: transparency (0.5 = 50% opaque)
    plt.grid(True, linestyle="--", alpha=0.5)
    
    # Display the legend (shows what each line represents)
    plt.legend()
    
    # Adjust the layout to prevent labels from being cut off
    plt.tight_layout()
    
    # Display the graph
    plt.show()


# This code runs when the script is executed directly (not imported)
if __name__ == "__main__":
    main()
