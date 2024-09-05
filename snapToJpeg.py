import os

def convertToJpeg(filePath, marker):
    # Convert the marker (hex string) to bytes
    markerBytes = bytes.fromhex(marker.replace(" ", ""))
    # Get the directory and filename
    directory, fileName = os.path.split(filePath)
    # Create the new output file name with "_img" suffix
    outputFileName = f"{fileName}_img.jpeg"
    outputFilePath = os.path.join(directory, outputFileName)

    with open(filePath, 'rb') as file:
        fileData = file.read()
        markerPosition = fileData.find(markerBytes)
        if markerPosition == -1:
            print(f"{fileName} is not a valid Snapmatic file. Skipping...")
            return
        # Slice the file data from the marker onwards
        trimmedData = fileData[markerPosition:]
        # Write the trimmed data to the output file in the same directory
        with open(outputFilePath, 'wb') as outputFile:
            outputFile.write(trimmedData)

    print(f"{fileName} has been successfully converted into jpeg.")

def processPGTAFiles(directory, marker):
    for fileName in os.listdir(directory):
        if fileName.startswith('PGTA'):
            filePath = os.path.join(directory, fileName)
            # Process the file
            convertToJpeg(filePath, marker)

if __name__ == '__main__':
    directory = os.getcwd() 
    marker = "FF D8 FF E0 00 10 4A 46 49 46 00 01"
    processPGTAFiles(directory, marker)
