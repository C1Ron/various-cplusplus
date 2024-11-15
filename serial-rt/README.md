
# Header
- field 0: [startByte, totalSize, payloadSize, mscId]  
- field 1: [msgRequestId, msgResponseId, conversationId, senderId]  
- field 2: [numBlocks, seqId, commandType, errorCode]  
- field 3: [futureUse0, futureUse1, futureUse2, endByte]

# Payload
    commandType = RT_READ : payload = [registerId, Crc]
    commandType = RT_READ_REPLY: payload = [registerId, valueBytes, Crc]
    commandType = RT_WRITE: payload = [registerId, valueBytes, Crc]
    commandType = RT_WRITE_REPLY: payload = [Crc] (but placed at endByte)


