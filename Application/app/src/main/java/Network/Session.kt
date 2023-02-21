package Network

import java.net.Socket
import java.nio.ByteBuffer
import java.util.Queue
import Network.ClientService



open class Session
{

    enum class BufferSize(val size: Long)
    {
        KB_64(0x10000),
    }

    private var _service: ClientService? = null;
    private var _socket: Socket? = null;
    private var _netAddress: NetAddress? = null;
    private var _connected: Boolean = false;
    private var _recvBuffer: RecvBuffer = null;
    private var _sendQueue: Queue<SendBuffer>? = null;

    fun Send(sendBuffer: SendBuffer)
    {

    }

    fun Connect(): Boolean
    {

    }

    fun Disconnect()
    {

    }

    fun GetService(): ClientService
    {

    }

    fun SetService(service: ClientService)
    {

    }

    fun SetNetAddress(netAddress: NetAddress) { _netAddress = netAddress; }

    fun GetAddress(): NetAddress { return _netAddress; }

    fun GetSocket(): Socket { return _socket; }

    fun IsConnected(): Boolean { return _connected; }

    fun GetSession(): Session { return this; }

    fun RegisterConnect(): Boolean
    {

    }

    fun RegisterDisconnect(): Boolean
    {

    }

    fun RegisterRecv()
    {

    }

    fun RegisterSend()
    {

    }

    open fun OnConnected() { }

    open fun OnRecv(buffer: ByteBuffer, len: Int): Int { }

    open fun OnSend(len: Int) { }

    open fun OnDisconnected() { }



}



open class PacketSession: Session()
{
    fun GetPacketSession(): PacketSession { return this;}

    open fun OnRecvPacket(buffer: ByteBuffer, len: Int) { }

}