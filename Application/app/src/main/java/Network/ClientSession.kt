package Network

import java.nio.ByteBuffer

class ClientSession: PacketSession()
{

    override fun OnConnected()
    {
        println("On Connected");
        TODO("Not yet implemented")
    }

    override fun OnRecvPacket(buffer: ByteBuffer, len: Int)
    {
        ServerPacketHandler.HandlePacket(this, buffer, len);
        TODO("Not yet implemented")
    }

    override fun OnSend(len: Int)
    {
        println("On Send");
        TODO("Not yet implemented")
    }

    override fun OnDisconnected()
    {
        println("On Disconnected");
        TODO("Not yet implemented")
    }
}