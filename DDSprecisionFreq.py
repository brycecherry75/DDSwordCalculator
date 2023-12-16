import argparse

if __name__ == "__main__":
  parser = argparse.ArgumentParser(fromfile_prefix_chars='@')
  parser.add_argument("--reference", type=int, default=0, help="Reference frequency start in Hz")
  parser.add_argument("--refmultiplier", type=int, default=1, help="Reference multiplier (default: 1)")
  parser.add_argument("--referencesteps", type=int, default=0, help="Reference frequency steps in Hz")
  parser.add_argument("--frequency", type=float, default=0, help="Desired frequency in Hz")
  parser.add_argument("--ddsbits", type=int, default=0, help="DDS bits within valid range")
  parser.add_argument("--ddslimithalf", action='store_true', help="Maximum DDS value is ((2 ^ DDS bits) / 2) - 1")
  args = parser.parse_args()

  reference = args.reference
  refmultiplier = args.refmultiplier
  referencesteps = args.referencesteps
  frequency = args.frequency
  ddsbits = args.ddsbits

  ValidParameters = True

  if reference <= 0:
    ValidParameters = False
    print("ERROR: Reference is not specified or is zero or negative")
  if refmultiplier < 1:
    ValidParameters = False
    print("ERROR: Reference multiplier is not a minimum of 1")
  if referencesteps <= 0:
    ValidParameters = False
    print("ERROR: Reference steps is not specified or is zero or negative")
  if frequency <= 0:
    ValidParameters = False
    print("ERROR: Frequency is not specified or is zero or negative")
  if ddsbits < 8 or ddsbits > 48:
    ValidParameters = False
    print("ERROR: DDS bits is not specified or not within 8-48")
  if args.ddslimithalf == True and frequency >= (((reference + referencesteps) * refmultiplier) / 2):
    ValidParameters = False
    print("ERROR: Desiered frequency must be less than half reference including reference multiplier")
  elif frequency >= ((reference + referencesteps) * refmultiplier):
    ValidParameters = False
    print("ERROR: Desiered frequency must be less than reference including reference multiplier")

  if ValidParameters == True:
    MaximumDDSvalue = (1 << ddsbits)
    DDSrange = MaximumDDSvalue
    if args.ddslimithalf == True:
      DDSrange = int(MaximumDDSvalue / 2)
    BestDDSvalue = MaximumDDSvalue
    BestFrequencyError = reference
    BestReferenceFrequency = reference
    MatchFound = False
    for ReferenceStepsToTry in range (referencesteps):
      CurrentDDSvalue = int((MaximumDDSvalue * frequency) / ((reference + ReferenceStepsToTry) * refmultiplier))
      ActualFrequency = ((reference + ReferenceStepsToTry) * refmultiplier * CurrentDDSvalue) / MaximumDDSvalue
      FrequencyError = frequency - ActualFrequency
      if FrequencyError < 0:
        FrequencyError *= -1
      if FrequencyError < BestFrequencyError and CurrentDDSvalue < DDSrange:
        MatchFound = True
        BestFrequencyError = FrequencyError
        BestDDSvalue = CurrentDDSvalue
        BestReferenceFrequency = (reference + ReferenceStepsToTry)
        if FrequencyError == 0:
          break
    if MatchFound == True:
      print("Best possible frequency error is", BestFrequencyError, "Hz with DDS value of", hex(BestDDSvalue), "and input clock of", BestReferenceFrequency, "Hz")
    else:
      print("No match found")