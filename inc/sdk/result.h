
#ifndef AIRBRAKES_SDK_RESULT_H_
#define AIRBRAKES_SDK_RESULT_H_

/* passes the error upward, works for result and success */
#define RESULT_UNWRAP(x) auto memo = (x); if (!memo.is_ok()) { return memo; }

/* instead, pass another error */
#define RESULT_UNWRAP_OR(x, err) if (!x.is_ok()) { return err; }

namespace sdk {

enum class result_err {
    OK,
    FAIL
};

template<typename E>
struct success;

template<typename V, typename E = result_err>
struct result {

    V value;
    E err;

    result(V value, E err = E::OK) : value(value), err(err)
    {
    }

    result(E err) : err(err)
    {
    }

    result(success<E> succ) : err(succ.err)
    {
    }

    bool is_ok() 
    {
        return err == E::OK;
    }

    V unwrap()
    {
        return value;
    }
};

template<typename E = result_err>
struct success {
    
    E err;

    success(E err = E::OK) : err(err)
    {
    }

    // casting constructor
    template<typename V>
    success(result<V, E> res) : err(res.err)
    {
    }

    bool is_ok()
    {
        return err = E::OK;
    }
};

} // namespace sdk

#endif // AIRBRAKES_SDK_RESULT_H_
