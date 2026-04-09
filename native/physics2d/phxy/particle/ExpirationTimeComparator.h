#pragma once

namespace phxy
{
    // Compares the expiration time of two particle indices.
    class ExpirationTimeComparator
    {
    public:
        // Initialize the class with a pointer to an array of particle
        // lifetimes.
        ExpirationTimeComparator(const int *const expirationTimes) : m_expirationTimes(expirationTimes){}
        
        // Empty destructor.
        ~ExpirationTimeComparator() {}

        // Compare the lifetime of particleIndexA and particleIndexB
        // returning true if the lifetime of A is greater than B for particles
        // that will expire.  If either particle's lifetime is infinite (<= 0.0f)
        // this function return true if the lifetime of A is lesser than B.
        // When used with std::sort() this results in an array of particle
        // indicies sorted in reverse order by particle lifetime.
        // For example, the set of lifetimes
        // (1.0, 0.7, 0.3, 0.0, -1.0, -2.0)
        // would be sorted as
        // (0.0, -1.0, -2.0, 1.0, 0.7, 0.3)
        bool operator()(const int particleIndexA, const int particleIndexB) const
        {
            const int expirationTimeA = m_expirationTimes[particleIndexA];
            const int expirationTimeB = m_expirationTimes[particleIndexB];
            const bool infiniteExpirationTimeA = expirationTimeA <= 0.0f;
            const bool infiniteExpirationTimeB = expirationTimeB <= 0.0f;
            return infiniteExpirationTimeA == infiniteExpirationTimeB ? expirationTimeA > expirationTimeB : infiniteExpirationTimeA;
        }

    private:
        const int *m_expirationTimes;
    };
}