// Health Endpoint Pattern in Rust
// Expose system health for monitoring and load balancers

use std::collections::HashMap;
use std::time::Instant;

#[derive(Debug, Clone, PartialEq)]
enum HealthStatus {
    Healthy,
    Degraded,
    Unhealthy,
}

#[derive(Debug)]
struct ComponentHealth {
    name: String,
    status: HealthStatus,
    message: Option<String>,
    latency_ms: Option<u64>,
}

struct HealthChecker {
    checks: Vec<Box<dyn Fn() -> ComponentHealth>>,
}

impl HealthChecker {
    fn new() -> Self {
        HealthChecker { checks: Vec::new() }
    }

    fn add_check<F>(&mut self, check: F)
    where
        F: Fn() -> ComponentHealth + 'static,
    {
        self.checks.push(Box::new(check));
    }

    fn check_health(&self) -> HealthReport {
        let start = Instant::now();
        let components: Vec<_> = self.checks.iter().map(|check| check()).collect();

        let overall = if components.iter().any(|c| c.status == HealthStatus::Unhealthy) {
            HealthStatus::Unhealthy
        } else if components.iter().any(|c| c.status == HealthStatus::Degraded) {
            HealthStatus::Degraded
        } else {
            HealthStatus::Healthy
        };

        HealthReport {
            status: overall,
            components,
            total_ms: start.elapsed().as_millis() as u64,
        }
    }
}

struct HealthReport {
    status: HealthStatus,
    components: Vec<ComponentHealth>,
    total_ms: u64,
}

impl std::fmt::Display for HealthReport {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        writeln!(f, "Overall: {:?} ({}ms)", self.status, self.total_ms)?;
        for comp in &self.components {
            write!(f, "  {}: {:?}", comp.name, comp.status)?;
            if let Some(ms) = comp.latency_ms {
                write!(f, " ({}ms)", ms)?;
            }
            if let Some(ref msg) = comp.message {
                write!(f, " - {}", msg)?;
            }
            writeln!(f)?;
        }
        Ok(())
    }
}

fn main() {
    let mut checker = HealthChecker::new();

    // Database check
    checker.add_check(|| ComponentHealth {
        name: "database".into(),
        status: HealthStatus::Healthy,
        message: Some("Connection pool: 5/20 active".into()),
        latency_ms: Some(12),
    });

    // Cache check
    checker.add_check(|| ComponentHealth {
        name: "cache".into(),
        status: HealthStatus::Healthy,
        message: Some("Hit rate: 94%".into()),
        latency_ms: Some(2),
    });

    // External API check
    checker.add_check(|| ComponentHealth {
        name: "payment_api".into(),
        status: HealthStatus::Degraded,
        message: Some("Response time elevated".into()),
        latency_ms: Some(850),
    });

    // Disk check
    checker.add_check(|| ComponentHealth {
        name: "disk".into(),
        status: HealthStatus::Healthy,
        message: Some("45% used".into()),
        latency_ms: None,
    });

    let report = checker.check_health();
    println!("=== Health Check ===");
    print!("{}", report);
}
