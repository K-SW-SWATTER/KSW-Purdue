package Network
import java.nio.channels.Selector
import Network.NetAddress
import Network.Session


enum class ServiceType
{
    Server,
    Client
}

//typealias SessionFactory = Session();

class ClientService constructor(private val _type: ServiceType, private val _netAddress: NetAddress, private var _sessionFactory: Session, private val _maxSessionCount: Int)
{
    private val _selectorCore: SelectorCore? = null;
    private var _sessionCount : Int = 0;
    private var _sessions  = mutableSetOf<Session>();

    fun Start() : Boolean
    {
        if (CanStart() == false)
            return false;

        val sessionCount : Int = GetMaxSessionCount();
        for (i in 1.. sessionCount)
        {
            val session : Session = CreateSession();
            if (session.Connect() == false)
                return false;
        }

        return true;
    }
    fun CanStart() : Boolean { return _sessionFactory != null; }

    fun CloseService()
    {
        for (session in _sessions)
        {
            session.Disconnect();
        }
    }
    fun SetSessionFactory(func: Session) { _sessionFactory = func; }

    fun CreateSession() : Session
    {
        var session: Session = _sessionFactory;
    }

    fun AddSession(session: Session)
    {
        _sessionCount++;
        _sessions.add(session);
    }

    fun ReleaseSession(session : Session)
    {
        try
        {
            _sessions.remove(session);
            _sessionCount;
        }
        catch (e: java.lang.Exception)
        {
            // TODO
        }

    }
    fun GetCurrentSessionCount(): Int { return _sessionCount; }
    fun GetMaxSessionCount(): Int { return _maxSessionCount; }

    fun GetServiceType(): ServiceType { return _type; }
    fun GetNetAddress(): NetAddress { return _netAddress; }
    fun GetSelectorCore(): SelectorCore? { return _selectorCore; }

}