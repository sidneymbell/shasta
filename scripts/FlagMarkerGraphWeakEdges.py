#!/usr/bin/python3

import shasta
import GetConfig

# Read the config file.
config = GetConfig.getConfig()


# Initialize the assembler and access what we need.
a = shasta.Assembler()
a.accessMarkerGraphVertices()
a.accessMarkerGraphConnectivity(accessEdgesReadWrite=True)
a.flagMarkerGraphWeakEdges(
    lowCoverageThreshold = int(config['MarkerGraph']['lowCoverageThreshold']),
    highCoverageThreshold = int(config['MarkerGraph']['highCoverageThreshold']),
    maxDistance = int(config['MarkerGraph']['maxDistance']),
    )

